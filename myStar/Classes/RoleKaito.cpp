#include "RoleKaito.h"


RoleKaito::RoleKaito( )
{
	m_State = State::None ; 
}


RoleKaito::~RoleKaito( )
{
}

bool RoleKaito::init( )
{
	auto pTex = TextureCache::getInstance()->addImage( "kaito.png" ) ; 

	m_size.height = pTex->getContentSize( ).height / 4 ;
	m_size.width = pTex->getContentSize( ).width / 4 ;

	m_pSprMain = Sprite::create( "kaito.png" , Rect( m_size.width * 0 , m_size.height * 2 , m_size.width , m_size.height ) ) ;
	this->addChild( m_pSprMain ) ;
	m_pSprMain->setAnchorPoint( Point( 0.5 , 0.05 ) ) ;

	CreateActions( ) ; 

	SetState( State::WalkBottom ) ; 
	SetState( State::None ) ;

	return true ; 
}


void RoleKaito::CreateActions( )
{
	auto pTex = TextureCache::getInstance( )->addImage( "kaito.png" ) ;

	float DelayPerUnit = 0.15 ; 

	auto pAnim = Animation::create( ) ;
	pAnim->setDelayPerUnit( DelayPerUnit ) ;
	for ( int i = 0 ; i < 4 ; ++i )
	{
		Rect rect( m_size.width * i , m_size.height * 2 , m_size.width , m_size.height ) ;
		pAnim->addSpriteFrameWithTexture( pTex , rect ) ;
	}
	m_pActWalkRight = RepeatForever::create( Animate::create( pAnim ) ) ;
	m_Actions.pushBack( m_pActWalkRight ) ;


	pAnim = Animation::create( ) ;
	pAnim->setDelayPerUnit( DelayPerUnit ) ;
	for ( int i = 0 ; i < 4 ; ++i )
	{
		Rect rect( m_size.width * i , m_size.height * 1 , m_size.width , m_size.height ) ;
		pAnim->addSpriteFrameWithTexture( pTex , rect ) ;
	}
	m_pActWalkLeft = RepeatForever::create( Animate::create( pAnim ) ) ;
	m_Actions.pushBack( m_pActWalkLeft ) ;


	pAnim = Animation::create( ) ;
	pAnim->setDelayPerUnit( DelayPerUnit ) ;
	for ( int i = 0 ; i < 4 ; ++i )
	{
		Rect rect( m_size.width * i , m_size.height * 0 , m_size.width , m_size.height ) ;
		pAnim->addSpriteFrameWithTexture( pTex , rect ) ;
	}
	m_pActWalkBottom = RepeatForever::create( Animate::create( pAnim ) ) ;
	m_Actions.pushBack( m_pActWalkBottom ) ;


	pAnim = Animation::create( ) ;
	pAnim->setDelayPerUnit( DelayPerUnit ) ;
	for ( int i = 0 ; i < 4 ; ++i )
	{
		Rect rect( m_size.width * i , m_size.height * 3 , m_size.width , m_size.height ) ;
		pAnim->addSpriteFrameWithTexture( pTex , rect ) ;
	}
	m_pActWalkTop = RepeatForever::create( Animate::create( pAnim ) ) ;
	m_Actions.pushBack( m_pActWalkTop ) ;

}


void RoleKaito::SetState( State type )
{
	m_pSprMain->stopAllActions( ) ;

	switch ( type )
	{
	case State::None:
	{
		m_pSprMain->stopAllActions( ) ; 

		// 之前的状态决定空闲的精灵
		switch ( m_State )
		{
		case RoleKaito::State::None:
		break;

		case RoleKaito::State::WalkLeft:
		{
			m_pSprMain->setTextureRect( Rect( m_size.width * 0 , m_size.height * 1 , m_size.width , m_size.height ) ) ;
		}
		break;

		case RoleKaito::State::WalkRight:
		{
			m_pSprMain->setTextureRect( Rect( m_size.width * 0 , m_size.height * 2 , m_size.width , m_size.height ) ) ;
		}
		break;

		case RoleKaito::State::WalkTop:
		{
			m_pSprMain->setTextureRect( Rect( m_size.width * 0 , m_size.height * 3 , m_size.width , m_size.height ) ) ;
		}
		break;

		case RoleKaito::State::WalkBottom:
		{
			m_pSprMain->setTextureRect( Rect( m_size.width * 0 , m_size.height * 0 , m_size.width , m_size.height ) );
		}
		break;

		default:
		break;
		}

	}
	break;

	case RoleKaito::State::WalkLeft:
	{
		
		m_pSprMain->runAction( m_pActWalkLeft ) ; 
	}
	break;

	case RoleKaito::State::WalkRight:
	{
		
		m_pSprMain->runAction( m_pActWalkRight ) ;
	}
	break;

	case State::WalkTop:
	{
		m_pSprMain->setTextureRect( Rect( m_size.width * 0 , m_size.height * 0 , m_size.width , m_size.height ) );
		m_pSprMain->runAction( m_pActWalkTop ) ;
	}
	break; 

	case State::WalkBottom:
	{
		m_pSprMain->runAction( m_pActWalkBottom ) ;
	}
	break;

	default:
	break;
	}

	m_State = type ; 

}
