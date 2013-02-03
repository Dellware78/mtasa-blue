/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCheckpointSA.cpp
*  PURPOSE:     Checkpoint entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

VOID CCheckpointSA::SetPosition(CVector * vecPosition)
{
    DEBUG_TRACE("VOID CCheckpointSA::SetPosition(CVector * vecPosition)");
    MemCpy (&this->GetInterface()->m_pos, vecPosition, sizeof(CVector));
}

CVector * CCheckpointSA::GetPosition()
{
    DEBUG_TRACE("CVector * CCheckpointSA::GetPosition()");
    return &this->GetInterface()->m_pos;
}

VOID CCheckpointSA::SetPointDirection(CVector * vecPointDirection)
{
    DEBUG_TRACE("VOID CCheckpointSA::SetPointDirection(CVector * vecPointDirection)");
    MemCpy (&this->GetInterface()->m_pointDir, vecPointDirection, sizeof(CVector));
}

CVector * CCheckpointSA::GetPointDirection()
{
    DEBUG_TRACE("CVector * CCheckpointSA::GetPointDirection()");
    return &this->GetInterface()->m_pointDir;
}

DWORD CCheckpointSA::GetType()
{
    DEBUG_TRACE("DWORD CCheckpointSA::GetType()");
    return this->GetInterface()->m_nType;
}

VOID CCheckpointSA::SetType(WORD wType)
{
    DEBUG_TRACE("VOID CCheckpointSA::SetType(DWORD dwType)");
    this->GetInterface()->m_nType = wType;
}

BOOL CCheckpointSA::IsActive()
{
    DEBUG_TRACE("BOOL CCheckpointSA::IsActive()");
    return (BOOL)this->GetInterface()->m_bIsUsed;
}

VOID CCheckpointSA::Activate()
{
    DEBUG_TRACE("VOID CCheckpointSA::Activate()");
    this->GetInterface()->m_bIsUsed = true;
}

DWORD CCheckpointSA::GetIdentifier()
{
    DEBUG_TRACE("DWORD CCheckpointSA::GetIdentifier()");
    return this->GetInterface()->m_nIdentifier;
}

VOID CCheckpointSA::SetIdentifier(DWORD dwIdentifier)
{
    DEBUG_TRACE("VOID CCheckpointSA::SetIdentifier(DWORD dwIdentifier)");
    this->GetInterface()->m_nIdentifier = dwIdentifier;
}

SColor CCheckpointSA::GetColor()
{
    DEBUG_TRACE("RGBA CCheckpointSA::GetColor()");
    // From ABGR
    unsigned long ulABGR = this->GetInterface()->rwColour;
    SColor color;
    color.A = ( ulABGR >> 24 ) && 0xff;
    color.B = ( ulABGR >> 16 ) && 0xff;
    color.G = ( ulABGR >> 8 ) && 0xff;
    color.R = ulABGR && 0xff;
    return color;
}

VOID CCheckpointSA::SetColor(const SColor color)
{
    DEBUG_TRACE("VOID CCheckpointSA::SetColor(RGBA color)");
    // To ABGR
    this->GetInterface()->rwColour = ( color.A << 24 ) | ( color.B << 16 ) | ( color.G << 8 ) | color.R;
}

VOID CCheckpointSA::SetPulsePeriod(WORD wPulsePeriod)
{
    DEBUG_TRACE("VOID CCheckpointSA::SetPulsePeriod(WORD wPulsePeriod)");
    this->GetInterface()->m_nPulsePeriod = wPulsePeriod;
}

VOID CCheckpointSA::SetRotateRate(short RotateRate)
{
    DEBUG_TRACE("VOID CCheckpointSA::SetRotateRate(short RotateRate)");
    this->GetInterface()->m_nRotateRate = RotateRate;

    if ( RotateRate == 0 )
    {
        this->GetInterface()->m_rotFlag = true;
    }
    else
    {
        this->GetInterface()->m_rotFlag = false;
    }
}

FLOAT CCheckpointSA::GetSize()
{
    DEBUG_TRACE("FLOAT CCheckpointSA::GetSize()");
    return this->GetInterface()->m_fSize;
}

VOID CCheckpointSA::SetSize(FLOAT fSize)
{
    DEBUG_TRACE("VOID CCheckpointSA::SetSize(FLOAT fSize)");
    this->GetInterface()->m_fSize = fSize;
}

VOID CCheckpointSA::SetCameraRange(FLOAT fCameraRange)
{
    DEBUG_TRACE("VOID CCheckpointSA::SetCameraRange(FLOAT fCameraRange)");
    this->GetInterface()->m_fCameraRange = fCameraRange;
}

VOID CCheckpointSA::SetPulseFraction(FLOAT fPulseFraction)
{
    DEBUG_TRACE("VOID CCheckpointSA::SetPulseFraction(FLOAT fPulseFraction)");
    this->GetInterface()->m_fPulseFraction = fPulseFraction;
}

FLOAT CCheckpointSA::GetPulseFraction()
{
    DEBUG_TRACE("FLOAT CCheckpointSA::GetPulseFraction()");
    return this->GetInterface()->m_fPulseFraction;
}

VOID CCheckpointSA::Remove()
{
    DEBUG_TRACE("VOID CCheckpointSA::Remove()");
    this->GetInterface()->m_bIsUsed = false;
    this->GetInterface()->m_nType = 257;    
    this->GetInterface()->rwColour = 0;
    C3DMarkerSA * pMarker = (C3DMarkerSA * )(((C3DMarkersSA *)pGame->Get3DMarkers())->FindMarker ( this->GetIdentifier() ));
    if ( pMarker )
    {
        //pMarker->Disable();
        //pMarker->DeleteMarkerObject();
    }
    else
    {
        #ifdef _DEBUG
        //OutputDebugString ( "Couldn't find marker for checkpoint!" );
        #endif
    }
}