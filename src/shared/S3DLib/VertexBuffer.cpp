// I N C L U D E S ///////////////////////////////////
#include "VertexBuffer.h"



// F U N C T I O N S /////////////////////////////////


VertexBuffer::VertexBuffer() 
{ 
	m_VB = NULL; 
	m_nVertices = 0; 
	m_screenPtr = NULL;   
}

VertexBuffer::~VertexBuffer() 
{ 
	Cleanup(); 
}

//////// Init /////////////////////////////////
//
// Description:
//
// Initializes the vertex buffer with the 
// correct settings.
//
///////////////////////////////////////////////

bool VertexBuffer::Init  (Screen3D& Screen, DWORD FvF, int vSize,  int nVertex, bool ParticleBuffer, bool Dynamic, _D3DPRIMITIVETYPE Type, bool ForceSoftware)
{

	// Get rid of any old data hanging around
	Cleanup();

	//store relevant data
	m_screenPtr = &Screen;
	m_vertexSize = vSize;
	m_nVertices = nVertex;
	m_fvf = FvF;

	//check for bad input parameters
	if(nVertex <= 0 || vSize <= 0 )
		return false;
	

	//figure out where to put the vertex buffer
	DWORD Usage = 0 ;
	m_pool = D3DPOOL_MANAGED;

	if( ParticleBuffer )
		Dynamic = true;

	if(Dynamic)
	{
		Usage |= D3DUSAGE_DYNAMIC;
		Usage |= D3DUSAGE_WRITEONLY;
		m_pool = D3DPOOL_DEFAULT;
	}

	// Create the vertex buffer.
	HRESULT HR = Screen.D3DDevice->CreateVertexBuffer( nVertex*vSize,
                                                  Usage , m_fvf,
                                                  m_pool  , &m_VB, NULL );
	
    if( FAILED( HR ) )
    {
		OutputDXError(HR, " on VertexBuffer::Init:  ");
        return false;
    }

	//wo0t!
    return true;

}



void VertexBuffer::FillBuffer( BYTE* VertexList, int nVert)
{   
	// Fill the vertex buffer. 
    BYTE* pVertices;
    
	if(VertexList)
	{
		if( FAILED( m_VB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
			return ;

		memcpy(pVertices, VertexList, m_vertexSize * nVert);

		//done with vertex buffer
		m_VB->Unlock();
	}

}


//////// Cleanup //////////////////////////////
//
// Description:
//
// Destroys any stuff hanging around.
//
///////////////////////////////////////////////

void VertexBuffer::Cleanup ()
{
	
	if(m_VB)
		m_VB->Release();

	m_VB = NULL;

}





//////// Lock /////////////////////////////////
//
// Description:
//
// Gives you a pointer that will allow you
// to modify the vertex buffer.
//
///////////////////////////////////////////////


bool VertexBuffer::Lock(BYTE** Vertex, DWORD Flags)
{
	if(!m_VB)
		return false;

	if( FAILED( m_VB->Lock( 0, 0, (void**)Vertex, Flags ) ) )
		return false;


	return true;

}



//////// Unlock ///////////////////////////////
//
// Description:
//
// Invalidates the pointer given to you from lock
//
///////////////////////////////////////////////

void VertexBuffer::Unlock()
{
	if(!m_VB)
		return;

	//done with vertex buffer
	m_VB->Unlock();
}




//////// CheckInput ///////////////////////////
//
// Description:
//
// Draws the contents of the vertex buffer. Note
// that this ignores any index buffers you may
// want to use.
//
///////////////////////////////////////////////

void VertexBuffer::Draw(Screen3D& Screen )
{
	if(!m_VB)
		return;

    // Render the vertex buffer contents
	HRESULT RVal;

	//Screen.SetMaterial(CurrentMaterial);
    SetActive( 0 );
    Screen.SetFVF( m_fvf );
    //Screen.D3DDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, VertexCount() / 3);

}




//////// operator = ///////////////////////////
//
// Description:
//
// Creates a duplicate with it's own unique
// memory (rather than simply copying pointers).
//
///////////////////////////////////////////////

void  VertexBuffer::operator = (  VertexBuffer& v ) 
{

	
}


/////////////////////////////////////////////////////////////////////



//////// Init /////////////////////////////////
//
// Description:
//
// Creates or recreates an index buffer.
//
///////////////////////////////////////////////

bool IndexBuffer::Init(Screen3D& Screen, int IndexCount, IndexBufferBitWidth bw)
{
	Destroy();

	//assert(IndexCount > 0);
	if(IndexCount <= 0 )
		return false ;

	nIndices = IndexCount;
	bitwidth = bw;
	D3DFORMAT fmt = D3DFMT_INDEX16;
	UINT bufferSize = IndexCount * sizeof(WORD);

	if(bitwidth == IB16Bit) {
		assert( IndexCount < 65536);
	}
	else {
		fmt = D3DFMT_INDEX32;
		bufferSize = IndexCount * sizeof(DWORD);
	}

	ScreenPtr = &Screen;

	HRESULT HR = Screen.D3DDevice->CreateIndexBuffer(bufferSize, NULL , fmt, D3DPOOL_MANAGED, &lpIndexBuffer, NULL);

	if(FAILED(HR))
	{
		OutputDebugString("CreateIndexBuffer failed");
		return false;
	}

	return true;
}


//////// Destroy //////////////////////////////
//
// Description:
//
// Frees up any memory used by the buffer.
//
///////////////////////////////////////////////

void IndexBuffer::Destroy()
{
	if(lpIndexBuffer)
		lpIndexBuffer->Release();

	lpIndexBuffer = NULL;

}

//////// operator = ///////////////////////////
//
// Description:
//
// Creates a duplicate with it's own unique
// memory (rather than simply copying pointers).
//
///////////////////////////////////////////////

void  IndexBuffer::operator = (  IndexBuffer& IB ) 
{

	/*if(nIndices != IB.nIndices)
	{

		//Get rid of old data
		Destroy();

		//recreate the vertex buffer
		nIndices = IB.nIndices;

		Init(*IB.ScreenPtr, nIndices);
	}
	
	//now lock the vertices from the new buffer, and
	//reinitialize our buffer to match it.
	UINT32* SrcList;
	UINT32* DstList;
	IB.Lock(&SrcList);
	Lock(&DstList);
	
	memcpy(DstList, SrcList, sizeof(UINT32) * nIndices);

	Unlock();
	IB.Unlock();*/
}




//////// Lock /////////////////////////////////
//
// Description:
//
// Gives you a pointer to the buffer.
//
///////////////////////////////////////////////

void IndexBuffer::Lock(Index16** Buffer)
{
	assert(bitwidth == IB16Bit);

	if(!lpIndexBuffer)
		return;

	HRESULT hr = lpIndexBuffer->Lock(0, nIndices*sizeof(Index16), (void**)Buffer, NULL);
}



//////// Lock /////////////////////////////////
//
// Description:
//
// Gives you a pointer to the buffer.
//
///////////////////////////////////////////////

void IndexBuffer::Lock(Index32** Buffer)
{
	assert(bitwidth == IB32Bit);
	if(!lpIndexBuffer)
		return;

	HRESULT hr = lpIndexBuffer->Lock(0, nIndices*sizeof(Index32), (void**)Buffer, NULL);
}

//////// Unlock ///////////////////////////////
//
// Description:
//
// Invalidates the pointer obtained from Lock.
//
///////////////////////////////////////////////

void IndexBuffer::Unlock()
{
	if(!lpIndexBuffer)
		return;

	lpIndexBuffer->Unlock();
}


