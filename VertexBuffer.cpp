#include "stdafx.h"
#include "VertexBuffer.h"
#include "RenderDevice.h"
#include "Shader.h"

static unsigned int fvfToSize(const DWORD fvf)
{
   switch (fvf)
   {
   case VertexFormat::VF_POS_NORMAL_TEX:
      return sizeof(Vertex3D_NoTex2);
   case VertexFormat::VF_POS_TEX:
      return sizeof(Vertex3D_TexelOnly);
   default:
      assert(false); // Unsupported FVF
      return 0;
   }
}

vector<VertexBuffer*> VertexBuffer::pendingSharedBuffers;

unsigned int VertexBuffer::GetPendingSharedBufferCount()
{
   unsigned int size = 0;
   for (VertexBuffer* buffer : pendingSharedBuffers)
      size += buffer->m_vertexCount;
   return size;
}

unsigned int VertexBuffer::GetSharedBufferVertexOffset(const unsigned int vertexCount, const VertexFormat fvf)
{
   if (pendingSharedBuffers.size() > 0 && pendingSharedBuffers[0]->m_vertexFormat != fvf)
      return 0;
   int pendingSharedBufferCount = GetPendingSharedBufferCount();
   if (pendingSharedBufferCount + vertexCount > 65535)
      return 0;
   return pendingSharedBufferCount;
}

VertexBuffer::VertexBuffer(RenderDevice* rd, const unsigned int vertexCount, const float* verts, const bool isDynamic, const VertexFormat fvf)
   : m_rd(rd)
   , m_vertexCount(vertexCount)
   , m_vertexFormat(fvf)
   , m_sizePerVertex(fvfToSize(fvf))
   , m_isStatic(!isDynamic)
   , m_size(fvfToSize(fvf) * vertexCount)
   , m_offset(isDynamic ? 0 : GetSharedBufferVertexOffset(vertexCount, fvf) * fvfToSize(fvf))
   , m_vertexOffset(isDynamic ? 0 : GetSharedBufferVertexOffset(vertexCount, fvf))
{
#ifndef __OPENGLES__
   // Disabled since OpenGL ES does not support glDrawElementsBaseVertex and we need it unless we remap the indices when creating the index buffer (and we should)
   if (m_isStatic)
   {
      if ((pendingSharedBuffers.size() > 0 && pendingSharedBuffers[0]->m_vertexFormat != m_vertexFormat) // Split on vertex format change
         || (m_vertexCount + GetPendingSharedBufferCount() > 65535)) // Split to avoid exceeding index buffer 16 bit limit
         CreatePendingSharedBuffer();
      pendingSharedBuffers.push_back(this);
   }
   else
#endif
   {
#if defined(ENABLE_SDL) // OpenGL
      glGenBuffers(1, &m_vb);
      glBindBuffer(GL_ARRAY_BUFFER, m_vb);
      glBufferData(GL_ARRAY_BUFFER, m_size, nullptr, m_isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

#else // DirectX 9
      // NB: We always specify WRITEONLY since MSDN states,
      // "Buffers created with D3DPOOL_DEFAULT that do not specify D3DUSAGE_WRITEONLY may suffer a severe performance penalty."
      // This means we cannot read from vertex buffers, but I don't think we need to.
      CHECKD3D(rd->GetCoreDevice()->CreateVertexBuffer(m_size, D3DUSAGE_WRITEONLY | (m_isStatic ? 0 : D3DUSAGE_DYNAMIC), 0, D3DPOOL_DEFAULT, &m_vb, nullptr));

#endif
   }
   if (verts != nullptr)
   {
      BYTE* const data = new BYTE[vertexCount * m_sizePerVertex];
      memcpy(data, verts, vertexCount * m_sizePerVertex);
      PendingUpload pending { 0, vertexCount * m_sizePerVertex, data };
      m_pendingUploads.push_back(pending);
   }
}

VertexBuffer::~VertexBuffer()
{
   if (!IsCreated())
      RemoveFromVectorSingle(pendingSharedBuffers, this);
   for (PendingUpload upload : m_pendingUploads)
      delete[] upload.data;
#if defined(ENABLE_SDL) // OpenGL
   if (m_sharedBufferRefCount != nullptr)
   {
      (*m_sharedBufferRefCount)--;
      if ((*m_sharedBufferRefCount) == 0)
      {
         delete m_sharedBufferRefCount;
         if (m_vb != 0)
            glDeleteBuffers(1, &m_vb);
      }
   }
   else if (m_vb != 0)
      glDeleteBuffers(1, &m_vb);
#else // DirectX 9
   SAFE_RELEASE(m_vb);
#endif
}

void VertexBuffer::lock(const unsigned int offsetToLock, const unsigned int sizeToLock, void **dataBuffer, const DWORD flags)
{
   assert(!m_isStatic || !m_uploaded);
   assert(m_lock.data == nullptr); // Lock is not reentrant
   m_rd->m_curLockCalls++;
   m_lock.offset = offsetToLock;
   m_lock.size = sizeToLock == 0 ? m_size : sizeToLock;
   m_lock.data = new BYTE[m_lock.size];
   *dataBuffer = m_lock.data;
}

void VertexBuffer::unlock()
{
   assert(m_lock.data != nullptr);
   PendingUpload upload = m_lock;
   m_pendingUploads.push_back(upload);
   m_lock.data = nullptr;
}

void VertexBuffer::CreatePendingSharedBuffer()
{
   if (pendingSharedBuffers.size() == 0)
      return;
   UINT size = GetPendingSharedBufferCount() * pendingSharedBuffers[0]->m_sizePerVertex;

   #if defined(ENABLE_SDL) // OpenGL
   UINT8* data = (UINT8*)malloc(size);

   #else // DirectX 9
   IDirect3DVertexBuffer9* vb = nullptr;
   CHECKD3D(pendingSharedBuffers[0]->m_rd->GetCoreDevice()->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, 0 /* pendingSharedBuffers[0]->m_vertexFormat */, D3DPOOL_DEFAULT, &vb, nullptr));
   UINT8* data;
   CHECKD3D(vb->Lock(0, size, (void**)&data, 0));
   #endif

   for (VertexBuffer* buffer : pendingSharedBuffers)
   {
      assert(buffer->m_vertexFormat == pendingSharedBuffers[0]->m_vertexFormat);
      for (size_t j = 0; j < buffer->m_pendingUploads.size(); j++)
      {
         PendingUpload& upload = buffer->m_pendingUploads[j];
         memcpy(data + buffer->m_offset + upload.offset, upload.data, upload.size);
         delete[] upload.data;
      }
      buffer->m_pendingUploads.clear();
      buffer->m_uploaded = true;
   }

   #if defined(ENABLE_SDL) // OpenGL
   GLuint vb = 0;
   glGenBuffers(1, &vb);
   glBindBuffer(GL_ARRAY_BUFFER, vb);
   glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
   free(data);
   int* refCount = new int();
   (*refCount) = (int)pendingSharedBuffers.size();
   for (VertexBuffer* buffer : pendingSharedBuffers)
   {
      buffer->m_vb = vb;
      buffer->m_sharedBufferRefCount = refCount;
   }

   #else ENABLE_SDL // DirectX 9
   CHECKD3D(vb->Unlock());
   for (VertexBuffer* buffer : pendingSharedBuffers)
   {
      buffer->m_vb = vb;
      vb->AddRef();
   }
   vb->Release();

   #endif
   pendingSharedBuffers.clear();
}

void VertexBuffer::Upload()
{
   if (!IsCreated())
      CreatePendingSharedBuffer();

   if (m_pendingUploads.size() > 0)
   {
      for (PendingUpload upload : m_pendingUploads)
      {
         #if defined(ENABLE_SDL) // OpenGL
         glBindBuffer(GL_ARRAY_BUFFER, m_vb);
         glBufferSubData(GL_ARRAY_BUFFER, m_offset + upload.offset, upload.size, upload.data);

         #else // DirectX 9
         // It would be better to perform a single lock but in fact, I don't think there are situations where more than one update is pending
         UINT8* data;
         CHECKD3D(m_vb->Lock(m_offset + upload.offset, upload.size, (void**)&data, 0));
         memcpy(data, upload.data, upload.size);
         CHECKD3D(m_vb->Unlock());

         #endif
         delete[] upload.data;
      }
      m_pendingUploads.clear();
      m_uploaded = true;
   }
}
