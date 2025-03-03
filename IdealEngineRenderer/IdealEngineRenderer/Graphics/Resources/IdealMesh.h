#pragma once
#include "Core/Core.h"
#include "D3D12/D3D12Thirdparty.h"
#include "D3D12/D3D12Resource.h"
#include <memory>

class CD3D12VertexBuffer;
class CD3D12IndexBuffer;
// 필요한 것
// VB, IB, 
// 
template <typename TVertexType>
class CIdealMesh
{
public:
	CIdealMesh()
		: VertexBuffer()
		, IndexBuffer() {}
	~CIdealMesh() = default;

public:
	void SetVertexBuffer(std::shared_ptr<CD3D12VertexBuffer> InVertexBuffer) { VertexBuffer = InVertexBuffer; }
	void SetIndexBuffer(std::shared_ptr<CD3D12IndexBuffer> InIndexBuffer) { IndexBuffer = InIndexBuffer; }

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return VertexBuffer->GetView(); }
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const { return IndexBuffer->GetView(); }
	const uint32& GetElementCount() const { return IndexBuffer->GetElementCount(); }

private:
	std::shared_ptr<CD3D12VertexBuffer> VertexBuffer;
	std::shared_ptr<CD3D12IndexBuffer> IndexBuffer;

};

