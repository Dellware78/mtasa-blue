/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CTileBatcher.cpp
*  PURPOSE:
*  DEVELOPERS:  vidiot
*
*
*****************************************************************************/

#include <StdInc.h>
#include "CTileBatcher.h"

////////////////////////////////////////////////////////////////
//
// CTileBatcher::CTileBatcher
//
//
//
////////////////////////////////////////////////////////////////
CTileBatcher::CTileBatcher ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::~CTileBatcher
//
//
//
////////////////////////////////////////////////////////////////
CTileBatcher::~CTileBatcher ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CTileBatcher::OnDeviceCreate ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY )
{
    m_pDevice = pDevice;

    // Cache matrices
    D3DXMatrixIdentity ( &m_MatView );

    D3DXMATRIX Mat1, Mat2;
    D3DXMatrixTranslation ( &Mat1, -fViewportSizeX / 2.0f - 0.5f, -fViewportSizeY / 2.0f - 0.5f, 0.0f );
    D3DXMatrixScaling ( &Mat2, 2.0f / fViewportSizeX, -2.0f / fViewportSizeY, 1.0f );
    D3DXMatrixMultiply ( &m_MatProjection, &Mat1, &Mat2 );
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::OnRenderTargetChange
//
//
//
////////////////////////////////////////////////////////////////
void CTileBatcher::OnChangingRenderTarget ( uint uiNewViewportSizeX, uint uiNewViewportSizeY )
{
    // Flush dx draws
    Flush ();

    // Make new projection transform
    float fViewportSizeX = uiNewViewportSizeX;
    float fViewportSizeY = uiNewViewportSizeY;
    D3DXMATRIX Mat1, Mat2;
    D3DXMatrixTranslation ( &Mat1, -fViewportSizeX / 2.0f - 0.5f, -fViewportSizeY / 2.0f - 0.5f, 0.0f );
    D3DXMatrixScaling ( &Mat2, 2.0f / fViewportSizeX, -2.0f / fViewportSizeY, 1.0f );
    D3DXMatrixMultiply ( &m_MatProjection, &Mat1, &Mat2 );
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::Flush
//
// Send all buffered vertices to D3D
//
////////////////////////////////////////////////////////////////
void CTileBatcher::Flush ( void )
{
    if( !m_Vertices.empty() )
    {
        //
        // TODO - Optimize all this
        //

        // Save state
        LPDIRECT3DSTATEBLOCK9   m_pDeviceState = NULL;
#if 0   // Creating a state block here may not be necessary
        m_pDevice->CreateStateBlock ( D3DSBT_ALL, &m_pDeviceState );
#endif

        // Set states
        m_pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_FALSE );
        m_pDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
        m_pDevice->SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
        m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
        m_pDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
        m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        m_pDevice->SetRenderState( D3DRS_ALPHAREF,         0x01 );
        m_pDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
        m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
        m_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
        m_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );


        // Set transforms
        D3DXMATRIX matWorld;
        if ( m_fCurrentRotation == 0.0f )
        {
            // No rotation
            D3DXMatrixIdentity ( &matWorld );
        }
        else
        {
            // Yes rotation
            D3DXMATRIX MatToCOR, MatRot, MatFromCOR;
            D3DXMatrixTranslation ( &MatToCOR, -m_fCurrentRotCenX, -m_fCurrentRotCenY, 0 );
            D3DXMatrixRotationZ ( &MatRot, m_fCurrentRotation * (6.2832f/360.f) );
            D3DXMatrixTranslation ( &MatFromCOR, m_fCurrentRotCenX, m_fCurrentRotCenY, 0 );

            // Mout = Mrc-1 * Mr * Mrc
            matWorld = MatToCOR * MatRot * MatFromCOR;
        }

        m_pDevice->SetTransform ( D3DTS_WORLD, &matWorld );
        m_pDevice->SetTransform ( D3DTS_VIEW, &m_MatView );
        m_pDevice->SetTransform ( D3DTS_PROJECTION, &m_MatProjection );

        // Set vertex stream
        uint PrimitiveCount                 = m_Vertices.size () / 3;
        const void* pVertexStreamZeroData   = &m_Vertices[0];
        uint VertexStreamZeroStride         = sizeof(SPDTVertex);
        m_pDevice->SetFVF ( SPDTVertex::FVF );

        // Draw
        if ( CTextureItem* pTextureItem = DynamicCast < CTextureItem > ( m_pCurrentMaterial ) )
        {
            // Draw using texture
            m_pDevice->SetTexture( 0, pTextureItem->m_pD3DTexture );
            m_pDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );
        }
        else
        if ( CShaderInstance* pShaderInstance = DynamicCast < CShaderInstance > ( m_pCurrentMaterial ) )
        {
            // Draw using shader
            ID3DXEffect* pD3DEffect = pShaderInstance->m_pEffectWrap->m_pD3DEffect;

            // Apply custom parameters
            pShaderInstance->ApplyShaderParameters ();
            // Apply common parameters
            pShaderInstance->m_pEffectWrap->ApplyCommonHandles ();
            // Apply mapped parameters
            pShaderInstance->m_pEffectWrap->ApplyMappedHandles ();

            // Do shader passes
            DWORD dwFlags = pShaderInstance->m_pEffectWrap->m_uiSaveStateFlags;      // D3DXFX_DONOTSAVE(SHADER|SAMPLER)STATE
            uint uiNumPasses = 0;
            pD3DEffect->Begin ( &uiNumPasses, dwFlags );

            for ( uint uiPass = 0 ; uiPass < uiNumPasses ; uiPass++ )
            {
                pD3DEffect->BeginPass ( uiPass );
                m_pDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );
                pD3DEffect->EndPass ();
            }
            pD3DEffect->End ();

            // If we didn't get the effect to save the shader state, clear some things here
            if ( dwFlags & D3DXFX_DONOTSAVESHADERSTATE )
            {
                m_pDevice->SetVertexShader( NULL );
                m_pDevice->SetPixelShader( NULL );
            }
        }

        // Clean up
        m_Vertices.clear ();
        SetCurrentMaterial ( NULL );
        m_fCurrentRotation = 0;
        m_fCurrentRotCenX = 0;
        m_fCurrentRotCenY = 0;

        // Restore state
        if ( m_pDeviceState )
        {
            m_pDeviceState->Apply ( );
            m_pDeviceState->Release ( );
            m_pDeviceState = NULL;
        }
    }
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::SetCurrentMaterial
//
// Safely change the current rendering material
//
////////////////////////////////////////////////////////////////
void CTileBatcher::SetCurrentMaterial ( CMaterialItem* pMaterial )
{
    if ( m_pCurrentMaterial )
        m_pCurrentMaterial->Release ();

    m_pCurrentMaterial = pMaterial;

    if ( m_pCurrentMaterial )
        m_pCurrentMaterial->AddRef ();
}


////////////////////////////////////////////////////////////////
//
// CTileBatcher::AddTile
//
// Add a new tile to the buffer.
// Will flush previous tiles if the material is changing and such
//
////////////////////////////////////////////////////////////////
void CTileBatcher::AddTile ( float fX1, float fY1,
                              float fX2, float fY2,
                              float fU1, float fV1,
                              float fU2, float fV2, 
                              CMaterialItem* pMaterial,
                              float fRotation,
                              float fRotCenOffX,
                              float fRotCenOffY,
                              unsigned long ulColor )
{

    // TODO - Optimize these checks
    float fRotCenX = fRotCenOffX + ( fX1 + fX2 ) * 0.5f;
    float fRotCenY = fRotCenOffY + ( fY1 + fY2 ) * 0.5f;

    // Check if we need to flush what has been done so far
    if ( pMaterial != m_pCurrentMaterial
            || fRotation != m_fCurrentRotation
            || fRotCenX != m_fCurrentRotCenX
            || fRotCenY != m_fCurrentRotCenY )
    {
        Flush ();
        SetCurrentMaterial ( pMaterial );
        m_fCurrentRotation = fRotation;
        m_fCurrentRotCenX = fRotCenX;
        m_fCurrentRotCenY = fRotCenY;
    }

    // TODO - Add tessellation option

    // Make room for 6 more vertices
    m_Vertices.resize ( m_Vertices.size () + 6 );
    SPDTVertex* pBuffer = &m_Vertices[m_Vertices.size () - 6];

    // Fill vertex data
    WRITE_PDT_VERTEX( pBuffer, fX1, fY1, 0, ulColor, fU1 , fV1 );
    WRITE_PDT_VERTEX( pBuffer, fX2, fY1, 0, ulColor, fU2 , fV1 );
    WRITE_PDT_VERTEX( pBuffer, fX1, fY2, 0, ulColor, fU1 , fV2 );

    WRITE_PDT_VERTEX( pBuffer, fX2, fY1, 0, ulColor, fU2 , fV1 );
    WRITE_PDT_VERTEX( pBuffer, fX2, fY2, 0, ulColor, fU2 , fV2 );
    WRITE_PDT_VERTEX( pBuffer, fX1, fY2, 0, ulColor, fU1 , fV2 );
}