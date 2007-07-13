<<<<<<< .mine
/*
* This source file is part of Rastullahs Lockenpracht.
* Copyright (C) 2003-2007 Team Pantheon. http://www.team-pantheon.de
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the Perl Artistic License.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  Perl Artistic License for more details.
*
*  You should have received a copy of the Perl Artistic License
*  along with this program; if not you can get it here
*  http://www.perldoc.com/perl5.6/Artistic.html.
*/
#include "stdinc.h" //precompiled header

#include "DialogControlState.h"
#include "Exception.h"

#include "Actor.h"
#include "CameraObject.h"
#include "CommandMapper.h"
#include "ConfigurationManager.h"
#include "CoreSubsystem.h"
#include "DialogCharacter.h"
#include "DialogSubsystem.h"
#include "DialogWindow.h"
#include "InputManager.h"
#include "MeshAnimation.h"
#include "MeshObject.h"
#include "Person.h"
#include "PhysicalThing.h"
#include "Sound.h"
#include "SoundDriver.h"
#include "SoundManager.h"
#include "SoundObject.h"
#include "SubtitleWindow.h"
#include "WindowFactory.h"
#include "WindowManager.h"
#include "World.h"

#include <OgreSceneManager.h>
#include <OgreNewt_Body.h>

using namespace Ogre;

namespace rl {

    DialogControlState::DialogControlState(CommandMapper* cmdMapper,
        Actor* camera, Person* character)
        : ControlState(cmdMapper, camera, character, CST_DIALOG),
        mTargetCameraPosition(Vector3::ZERO),
        mTargetCameraDirection(Vector3::UNIT_Z),
        mCurrFadeTextTime(0),
        mTotalFadeTextTime(0),
        mText(),
        mDialogMode(DM_FRONT),
        mTextShown(false),
        mDialogWindow(NULL),
        mSubtitleWindow(NULL),
        mSoundObject(NULL),
        mTalkAnimation(NULL),
        mSubtitleSpeed(1.0f)
    {
        mSubtitleSpeed = ConfigurationManager::getSingleton().getRealSetting(
            "General", "Subtitle Speed");

    }

    DialogControlState::~DialogControlState()
    {
        delete mSoundObject;
    }

    void DialogControlState::pause()
    {
        if(mDialogPartner)
        {
            MeshObject* mesh = dynamic_cast<MeshObject*>(mDialogPartner->getControlledObject());
            if (mesh != NULL)
            {
                mesh->stopAllAnimations();
                if (mesh->hasAnimation("idle"))
                {
                    mTalkAnimation = mesh->startAnimation("idle");
                }
            }
        }
        mCameraActor->getPhysicalThing()->unfreeze();
        mCharacterActor->getPhysicalThing()->unfreeze();
        mDialogWindow->setVisible(false, false);
        mSubtitleWindow->setVisible(false, false);
    }

    void DialogControlState::resume()
    {
        mCharacterActor->setVisible(true);

        mCameraActor->getPhysicalThing()->freeze();
        mCharacterActor->getPhysicalThing()->freeze();
        Camera* ogreCam = static_cast<Camera*>(mCameraActor->_getMovableObject());
        ogreCam->setFixedYawAxis(true);
        ogreCam->setPosition(Vector3::ZERO);
        ogreCam->setOrientation(Quaternion::IDENTITY);
        mCameraActor->_getSceneNode()->setFixedYawAxis(true);

        DialogCharacter* bot = DialogSubsystem::getSingleton().getCurrentDialogCharacter();
        bot->setDialogCharacter(mCharacter);
		mDialogPartner = bot->getDialogPartner()->getActor();

        recalculateCamera( mDialogPartner, mCharacterActor );

        mSubtitleWindow = WindowFactory::getSingleton().getSubtitleWindow();

        mDialogWindow = WindowFactory::getSingleton().getDialogWindow();
        mDialogWindow->initialize(this, bot);
		mDialogWindow->start();
    }

    void DialogControlState::run(Real elapsedTime)
    {
        if( elapsedTime <= 0.0 )
            return;

        // Aktuellen Abstand zur gew�nschten Position berechnen
        Vector3 posDistance = (mTargetCameraPosition - mCameraActor->_getSceneNode()->getPosition());
        // Ist eine Anpassung n�tig?
        if( posDistance != Vector3::ZERO )
        {
            // Die Kameraanpassgeschwindigkeit
            Real speed = 1/elapsedTime;
            mCameraActor->setPosition(mTargetCameraPosition);
        }

        mCameraActor->setOrientation( Quaternion::IDENTITY );
        mCameraActor->_getSceneNode()->setDirection( mTargetCameraDirection );
        mCameraActor->_getSceneNode()->_update( true, false );

        // Textanzeigedauer
        if (mCurrFadeTextTime >= 0)
        {
            mCurrFadeTextTime -= elapsedTime;
        }

        if (mTextShown && mCurrFadeTextTime <= 0)
        {
            mCurrFadeTextTime = 0;
            mTotalFadeTextTime = 0;
            mTextShown = false;
               if (mSoundObject)
            {
                mSoundObject->stop();
            }
            mSubtitleWindow->setVisible(false, false);
            mDialogWindow->textFinished();
        }

        LOG_DEBUG(Logger::UI,
                StringConverter::toString(mSubtitleWindow->getWindow()->getEffectiveAlpha())
                + ": "
                + mSubtitleWindow->getText()
                + CeGuiString(mSubtitleWindow->getWindow()->isVisible() ? "vis" : "unvis")
                + ", Time: "
                + StringConverter::toString(mCurrFadeTextTime));
    }

    void DialogControlState::recalculateCamera( Actor* speaker, Actor* listener )
    {
        // Position camera at position between char and dialog partner
        Vector3 charEyes = speaker->getWorldPosition();
        // Modify by MeshBounds
        if( speaker->getControlledObject()->isMeshObject() )
        {
            MeshObject* mo = dynamic_cast<MeshObject*>(speaker->getControlledObject());
            Ogre::AxisAlignedBox aab = mo->getDefaultSize();
            Vector3 offset(
                aab.getCenter().x,
                aab.getMaximum().y*0.933,
                aab.getCenter().z );
            charEyes += speaker->getWorldOrientation()*offset;
        }

        Vector3 partEyes = listener->getWorldPosition();
        // Modify by MeshBounds
        if( listener->getControlledObject()->isMeshObject() )
        {
            MeshObject* mo = dynamic_cast<MeshObject*>(listener->getControlledObject());
            Ogre::AxisAlignedBox aab = mo->getDefaultSize();
            Vector3 offset(
                aab.getCenter().x,
                aab.getMaximum().y*0.933,
                aab.getCenter().z );
            partEyes += listener->getWorldOrientation()*offset;
        }

        Vector3 globalCameraPosition = ( charEyes + partEyes ) / 2.0f;

        // Weltkoordinaten in lokale umwandeln
        mTargetCameraPosition =
            -1*(mCameraActor->_getSceneNode()->getParentSceneNode()->getWorldOrientation().Inverse()*
            (mCameraActor->_getSceneNode()->getParentSceneNode()->getWorldPosition() - globalCameraPosition));


        mTargetCameraDirection = ( partEyes - mTargetCameraPosition ).normalisedCopy();
    }

    float DialogControlState::getShowTextLength(const CeGuiString& text) const
    {
        return 0.019f * text.length() + // Zeit f�rs Text lesen
               0.25f;                   // Fade in
    }

    void DialogControlState::response(
        Actor* actor, const CeGuiString& text, const Ogre::String& soundFile)
    {
        if( actor == mDialogPartner )
            recalculateCamera( mCharacterActor, mDialogPartner );
        else
            recalculateCamera( mDialogPartner, mCharacterActor );

        // Ungef�hre Lesedauer bestimmen
        float fadeTime = getShowTextLength(text);
        if (soundFile.length() == 0)
        {
            float speed = mSubtitleSpeed;

            if( mSubtitleSpeed == 0.0 )
                speed = 1.0;

            mCurrFadeTextTime = fadeTime*speed;
            mTotalFadeTextTime = fadeTime*speed;
        }
        else
        {
            if( mSoundObject != NULL )
                mSoundObject->getMovableObject()->getParentSceneNode()->detachObject(
                    mSoundObject->getMovableObject() );

            delete mSoundObject;
            mSoundObject = new SoundObject(SoundManager::getSingleton().getActiveDriver()->
                    createSound(soundFile, ST_SAMPLE), soundFile);

            // An Sprecher h�ngen
            actor->_getSceneNode()->attachObject( mSoundObject->getMovableObject() );
            actor->_getSceneNode()->_update( true, false );
            mSoundObject->_setActor( actor );
            mSoundObject->set3d(true);
            mSoundObject->play();
            mSoundObject->_update();


            mCurrFadeTextTime = std::max(fadeTime*mSubtitleSpeed,mSoundObject->getLength());
            mTotalFadeTextTime = mCurrFadeTextTime;
        }

        MeshObject* mesh = dynamic_cast<MeshObject*>(actor->getControlledObject());
        if (mesh != NULL)
        {
            if (mesh->hasAnimation("reden"))
            {
                mesh->stopAllAnimations();
                mTalkAnimation = mesh->startAnimation("reden");
            }
        }

        LOG_DEBUG(Logger::UI,
            "Response: "
                + actor->getName()
                + " File: '"
                + soundFile
                + "', Text: '" + text + "', Time: "
                + StringConverter::toString(mCurrFadeTextTime));


        mTextShown = true;

        if (mSubtitleWindow != NULL && !text.empty())
        {
            mSubtitleWindow->show(text);
        }
    }

    bool DialogControlState::mouseReleased(const OIS::MouseEvent& evt,
        OIS::MouseButtonID id)
    {

        if (mTextShown && (mCurrFadeTextTime + 0.25) < mTotalFadeTextTime)
        {
            mCurrFadeTextTime = -1;

            return true;
        }
        else
        {
            return false;
        }

    }
}
=======
��/ *  
 *   T h i s   s o u r c e   f i l e   i s   p a r t   o f   R a s t u l l a h s   L o c k e n p r a c h t .  
 *   C o p y r i g h t   ( C )   2 0 0 3 - 2 0 0 7   T e a m   P a n t h e o n .   h t t p : / / w w w . t e a m - p a n t h e o n . d e  
 *  
 *     T h i s   p r o g r a m   i s   f r e e   s o f t w a r e ;   y o u   c a n   r e d i s t r i b u t e   i t   a n d / o r   m o d i f y  
 *     i t   u n d e r   t h e   t e r m s   o f   t h e   P e r l   A r t i s t i c   L i c e n s e .  
 *  
 *     T h i s   p r o g r a m   i s   d i s t r i b u t e d   i n   t h e   h o p e   t h a t   i t   w i l l   b e   u s e f u l ,  
 *     b u t   W I T H O U T   A N Y   W A R R A N T Y ;   w i t h o u t   e v e n   t h e   i m p l i e d   w a r r a n t y   o f  
 *     M E R C H A N T A B I L I T Y   o r   F I T N E S S   F O R   A   P A R T I C U L A R   P U R P O S E .     S e e   t h e  
 *     P e r l   A r t i s t i c   L i c e n s e   f o r   m o r e   d e t a i l s .  
 *  
 *     Y o u   s h o u l d   h a v e   r e c e i v e d   a   c o p y   o f   t h e   P e r l   A r t i s t i c   L i c e n s e  
 *     a l o n g   w i t h   t h i s   p r o g r a m ;   i f   n o t   y o u   c a n   g e t   i t   h e r e  
 *     h t t p : / / w w w . p e r l d o c . c o m / p e r l 5 . 6 / A r t i s t i c . h t m l .  
 * /  
  
 # i n c l u d e   " D i a l o g C o n t r o l S t a t e . h "  
 # i n c l u d e   " E x c e p t i o n . h "  
  
 # i n c l u d e   " A c t o r . h "  
 # i n c l u d e   " C a m e r a O b j e c t . h "  
 # i n c l u d e   " C o m m a n d M a p p e r . h "  
 # i n c l u d e   " C o n f i g u r a t i o n M a n a g e r . h "  
 # i n c l u d e   " C o r e S u b s y s t e m . h "  
 # i n c l u d e   " D i a l o g C h a r a c t e r . h "  
 # i n c l u d e   " D i a l o g O p t i o n . h "  
 # i n c l u d e   " D i a l o g R e s p o n s e . h "  
 # i n c l u d e   " D i a l o g S u b s y s t e m . h "  
 # i n c l u d e   " D i a l o g W i n d o w . h "  
 # i n c l u d e   " G a m e L o g g e r W i n d o w . h "  
 # i n c l u d e   " I n p u t M a n a g e r . h "  
 # i n c l u d e   " M e s h A n i m a t i o n . h "  
 # i n c l u d e   " M e s h O b j e c t . h "  
 # i n c l u d e   " P e r s o n . h "  
 # i n c l u d e   " P h y s i c a l T h i n g . h "  
 # i n c l u d e   " S o u n d . h "  
 # i n c l u d e   " S o u n d D r i v e r . h "  
 # i n c l u d e   " S o u n d M a n a g e r . h "  
 # i n c l u d e   " S o u n d O b j e c t . h "  
 # i n c l u d e   " S u b t i t l e W i n d o w . h "  
 # i n c l u d e   " W i n d o w F a c t o r y . h "  
 # i n c l u d e   " W i n d o w M a n a g e r . h "  
 # i n c l u d e   " W o r l d . h "  
  
 # i n c l u d e   < O g r e S c e n e M a n a g e r . h >  
 # i n c l u d e   < O g r e N e w t _ B o d y . h >  
  
 u s i n g   n a m e s p a c e   O g r e ;  
  
 n a m e s p a c e   r l   {  
  
 	 c o n s t   C e G u i S t r i n g   D i a l o g C o n t r o l S t a t e : : D I A L O G _ S T A R T   =   " 1 " ;  
 	 c o n s t   C e G u i S t r i n g   D i a l o g C o n t r o l S t a t e : : D I A L O G _ E N D   =   " D I A L O G   B E E N D E T " ;  
 	 c o n s t   C e G u i S t r i n g   D i a l o g C o n t r o l S t a t e : : D I A L O G _ E X I T   =   " E X I T " ;  
  
         D i a l o g C o n t r o l S t a t e : : D i a l o g C o n t r o l S t a t e ( C o m m a n d M a p p e r *   c m d M a p p e r ,  
                 A c t o r *   c a m e r a ,   P e r s o n *   c h a r a c t e r )  
                 :   C o n t r o l S t a t e ( c m d M a p p e r ,   c a m e r a ,   c h a r a c t e r ,   C S T _ D I A L O G ) ,  
                 m T a r g e t C a m e r a P o s i t i o n ( V e c t o r 3 : : Z E R O ) ,  
                 m T a r g e t C a m e r a D i r e c t i o n ( V e c t o r 3 : : U N I T _ Z ) ,  
                 m C u r r F a d e T e x t T i m e ( 0 ) ,  
                 m T o t a l F a d e T e x t T i m e ( 0 ) ,  
                 m T e x t ( ) ,  
                 m D i a l o g M o d e ( D M _ F R O N T ) ,  
                 m T e x t S h o w n ( f a l s e ) ,  
                 m D i a l o g W i n d o w ( N U L L ) ,  
                 m S u b t i t l e W i n d o w ( N U L L ) ,  
                 m S o u n d O b j e c t ( N U L L ) ,  
                 m T a l k A n i m a t i o n ( N U L L ) ,  
                 m S u b t i t l e S p e e d ( 1 . 0 f ) ,  
 	 	 m C u r r e n t R e s p o n s e ( N U L L ) ,  
 	 	 m C u r r e n t R e s p o n s e T e x t ( " " ) ,  
 	 	 m B o t ( N U L L ) ,  
 	 	 m G a m e L o g g e r ( N U L L )  
         {  
                 m S u b t i t l e S p e e d   =   C o n f i g u r a t i o n M a n a g e r : : g e t S i n g l e t o n ( ) . g e t R e a l S e t t i n g (  
                         " G e n e r a l " ,   " S u b t i t l e   S p e e d " ) ;  
         }  
  
         D i a l o g C o n t r o l S t a t e : : ~ D i a l o g C o n t r o l S t a t e ( )  
         {  
                 d e l e t e   m S o u n d O b j e c t ;  
         }  
  
         v o i d   D i a l o g C o n t r o l S t a t e : : p a u s e ( )  
         {  
                 i f   ( m D i a l o g P a r t n e r )  
                 {  
                         M e s h O b j e c t *   m e s h   =   d y n a m i c _ c a s t < M e s h O b j e c t * > ( m D i a l o g P a r t n e r - > g e t C o n t r o l l e d O b j e c t ( ) ) ;  
                         i f   ( m e s h   ! =   N U L L )  
                         {  
                                 m e s h - > s t o p A l l A n i m a t i o n s ( ) ;  
                                 i f   ( m e s h - > h a s A n i m a t i o n ( " i d l e " ) )  
                                 {  
                                         m T a l k A n i m a t i o n   =   m e s h - > s t a r t A n i m a t i o n ( " i d l e " ) ;  
                                 }  
                         }  
                 }  
                 m C a m e r a A c t o r - > g e t P h y s i c a l T h i n g ( ) - > u n f r e e z e ( ) ;  
                 m C h a r a c t e r A c t o r - > g e t P h y s i c a l T h i n g ( ) - > u n f r e e z e ( ) ;  
                 m D i a l o g W i n d o w - > s e t V i s i b l e ( f a l s e ,   f a l s e ) ;  
                 m S u b t i t l e W i n d o w - > s e t V i s i b l e ( f a l s e ,   f a l s e ) ;  
         }  
  
         v o i d   D i a l o g C o n t r o l S t a t e : : r e s u m e ( )  
         {  
                 m C h a r a c t e r A c t o r - > s e t V i s i b l e ( t r u e ) ;  
  
                 m C a m e r a A c t o r - > g e t P h y s i c a l T h i n g ( ) - > f r e e z e ( ) ;  
                 m C h a r a c t e r A c t o r - > g e t P h y s i c a l T h i n g ( ) - > f r e e z e ( ) ;  
                 C a m e r a *   o g r e C a m   =   s t a t i c _ c a s t < C a m e r a * > ( m C a m e r a A c t o r - > _ g e t M o v a b l e O b j e c t ( ) ) ;  
                 o g r e C a m - > s e t F i x e d Y a w A x i s ( t r u e ) ;  
                 o g r e C a m - > s e t P o s i t i o n ( V e c t o r 3 : : Z E R O ) ;  
                 o g r e C a m - > s e t O r i e n t a t i o n ( Q u a t e r n i o n : : I D E N T I T Y ) ;  
                 m C a m e r a A c t o r - > _ g e t S c e n e N o d e ( ) - > s e t F i x e d Y a w A x i s ( t r u e ) ;  
  
                 m D i a l o g W i n d o w   =   n e w   D i a l o g W i n d o w ( t h i s ) ;  
                 m S u b t i t l e W i n d o w   =   n e w   S u b t i t l e W i n d o w ( ) ;  
  
 	 	 m G a m e L o g g e r   =   W i n d o w F a c t o r y : : g e t S i n g l e t o n ( ) . g e t G a m e L o g g e r ( ) ;  
  
 	 	 D i a l o g C h a r a c t e r *   b o t   =   D i a l o g S u b s y s t e m : : g e t S i n g l e t o n ( ) . g e t C u r r e n t D i a l o g C h a r a c t e r ( ) ;  
 	 	 i n i t i a l i z e ( b o t ) ;  
         }  
  
 	 v o i d   D i a l o g C o n t r o l S t a t e : : i n i t i a l i z e ( D i a l o g C h a r a c t e r *   b o t )  
 	 {  
 	 	 m B o t   =   b o t ;  
                 m B o t - > s e t D i a l o g C h a r a c t e r ( m C h a r a c t e r ) ;  
 	 	 m D i a l o g P a r t n e r   =   m B o t - > g e t D i a l o g P a r t n e r ( ) - > g e t A c t o r ( ) ;  
  
                 r e c a l c u l a t e C a m e r a (   m D i a l o g P a r t n e r ,   m C h a r a c t e r A c t o r   ) ;  
  
 	 	 O g r e : : S t r i n g   v o i c e F i l e   =   m B o t - > g e t V o i c e F i l e ( ) . c _ s t r ( ) ;  
 	 	 i f   ( v o i c e F i l e   ! =   " " )  
 	 	 {  
 	 	 	 i f   ( v o i c e F i l e . f i n d ( " . z i p " )   ! =   O g r e : : S t r i n g : : n p o s )  
 	 	 	 {  
 	 	 	 	 R e s o u r c e G r o u p M a n a g e r : : g e t S i n g l e t o n ( ) . a d d R e s o u r c e L o c a t i o n (  
 	 	 	 	 	 v o i c e F i l e ,   " Z i p " ,   v o i c e F i l e ) ;  
 	 	 	 }  
 	 	 	 e l s e  
 	 	 	 {  
 	 	 	 	 R e s o u r c e G r o u p M a n a g e r : : g e t S i n g l e t o n ( ) . a d d R e s o u r c e L o c a t i o n (  
                       	 	 	 v o i c e F i l e ,   " F i l e S y s t e m " ,   v o i c e F i l e ) ;  
 	 	 	 }  
 	 	 }  
  
 	 	 s t a r t ( ) ;  
 	 }  
  
         v o i d   D i a l o g C o n t r o l S t a t e : : r u n ( R e a l   e l a p s e d T i m e )  
         {  
                 i f   (   e l a p s e d T i m e   < =   0 . 0   )  
                         r e t u r n ;  
  
                 / /   A k t u e l l e n   A b s t a n d   z u r   g e w � n s c h t e n   P o s i t i o n   b e r e c h n e n  
                 V e c t o r 3   p o s D i s t a n c e   =   ( m T a r g e t C a m e r a P o s i t i o n   -   m C a m e r a A c t o r - > _ g e t S c e n e N o d e ( ) - > g e t P o s i t i o n ( ) ) ;  
                 / /   I s t   e i n e   A n p a s s u n g   n � t i g ?  
                 i f   (   p o s D i s t a n c e   ! =   V e c t o r 3 : : Z E R O   )  
                 {  
                         / /   D i e   K a m e r a a n p a s s g e s c h w i n d i g k e i t  
                         R e a l   s p e e d   =   1 / e l a p s e d T i m e ;  
                         m C a m e r a A c t o r - > s e t P o s i t i o n ( m T a r g e t C a m e r a P o s i t i o n ) ;  
                 }  
  
                 m C a m e r a A c t o r - > s e t O r i e n t a t i o n (   Q u a t e r n i o n : : I D E N T I T Y   ) ;  
                 m C a m e r a A c t o r - > _ g e t S c e n e N o d e ( ) - > s e t D i r e c t i o n (   m T a r g e t C a m e r a D i r e c t i o n   ) ;  
                 m C a m e r a A c t o r - > _ g e t S c e n e N o d e ( ) - > _ u p d a t e (   t r u e ,   f a l s e   ) ;  
  
                 / /   T e x t a n z e i g e d a u e r  
                 i f   ( m C u r r F a d e T e x t T i m e   > =   0 )  
                 {  
                         m C u r r F a d e T e x t T i m e   - =   e l a p s e d T i m e ;  
                 }  
  
                 i f   ( m T e x t S h o w n   & &   m C u r r F a d e T e x t T i m e   < =   0 )  
                 {  
                         m C u r r F a d e T e x t T i m e   =   0 ;  
                         m T o t a l F a d e T e x t T i m e   =   0 ;  
                         m T e x t S h o w n   =   f a l s e ;  
                               i f   ( m S o u n d O b j e c t )  
                         {  
                                 m S o u n d O b j e c t - > s t o p ( ) ;  
                         }  
                         m S u b t i t l e W i n d o w - > s e t V i s i b l e ( f a l s e ,   f a l s e ) ;  
                         t e x t F i n i s h e d ( ) ;  
                 }  
  
                 L O G _ D E B U G ( L o g g e r : : U I ,  
                                 S t r i n g C o n v e r t e r : : t o S t r i n g ( m S u b t i t l e W i n d o w - > g e t W i n d o w ( ) - > g e t E f f e c t i v e A l p h a ( ) )  
                                 +   " :   "  
                                 +   m S u b t i t l e W i n d o w - > g e t T e x t ( )  
                                 +   C e G u i S t r i n g ( m S u b t i t l e W i n d o w - > g e t W i n d o w ( ) - > i s V i s i b l e ( )   ?   " v i s "   :   " u n v i s " )  
                                 +   " ,   T i m e :   "  
                                 +   S t r i n g C o n v e r t e r : : t o S t r i n g ( m C u r r F a d e T e x t T i m e ) ) ;  
         }  
  
         v o i d   D i a l o g C o n t r o l S t a t e : : r e c a l c u l a t e C a m e r a (   A c t o r *   s p e a k e r ,   A c t o r *   l i s t e n e r   )  
         {  
                 / /   P o s i t i o n   c a m e r a   a t   p o s i t i o n   b e t w e e n   c h a r   a n d   d i a l o g   p a r t n e r  
                 V e c t o r 3   c h a r E y e s   =   s p e a k e r - > g e t W o r l d P o s i t i o n ( ) ;  
                 / /   M o d i f y   b y   M e s h B o u n d s  
                 i f   (   s p e a k e r - > g e t C o n t r o l l e d O b j e c t ( ) - > i s M e s h O b j e c t ( )   )  
                 {  
                         M e s h O b j e c t *   m o   =   d y n a m i c _ c a s t < M e s h O b j e c t * > ( s p e a k e r - > g e t C o n t r o l l e d O b j e c t ( ) ) ;  
                         O g r e : : A x i s A l i g n e d B o x   a a b   =   m o - > g e t D e f a u l t S i z e ( ) ;  
                         V e c t o r 3   o f f s e t (  
                                 a a b . g e t C e n t e r ( ) . x ,  
                                 a a b . g e t M a x i m u m ( ) . y * 0 . 9 3 3 ,  
                                 a a b . g e t C e n t e r ( ) . z   ) ;  
                         c h a r E y e s   + =   s p e a k e r - > g e t W o r l d O r i e n t a t i o n ( ) * o f f s e t ;  
                 }  
  
                 V e c t o r 3   p a r t E y e s   =   l i s t e n e r - > g e t W o r l d P o s i t i o n ( ) ;  
                 / /   M o d i f y   b y   M e s h B o u n d s  
                 i f   (   l i s t e n e r - > g e t C o n t r o l l e d O b j e c t ( ) - > i s M e s h O b j e c t ( )   )  
                 {  
                         M e s h O b j e c t *   m o   =   d y n a m i c _ c a s t < M e s h O b j e c t * > ( l i s t e n e r - > g e t C o n t r o l l e d O b j e c t ( ) ) ;  
                         O g r e : : A x i s A l i g n e d B o x   a a b   =   m o - > g e t D e f a u l t S i z e ( ) ;  
                         V e c t o r 3   o f f s e t (  
                                 a a b . g e t C e n t e r ( ) . x ,  
                                 a a b . g e t M a x i m u m ( ) . y * 0 . 9 3 3 ,  
                                 a a b . g e t C e n t e r ( ) . z   ) ;  
                         p a r t E y e s   + =   l i s t e n e r - > g e t W o r l d O r i e n t a t i o n ( ) * o f f s e t ;  
                 }  
  
                 V e c t o r 3   g l o b a l C a m e r a P o s i t i o n   =   (   c h a r E y e s   +   p a r t E y e s   )   /   2 . 0 f ;  
  
                 / /   W e l t k o o r d i n a t e n   i n   l o k a l e   u m w a n d e l n  
                 m T a r g e t C a m e r a P o s i t i o n   =  
                         - 1 * ( m C a m e r a A c t o r - > _ g e t S c e n e N o d e ( ) - > g e t P a r e n t S c e n e N o d e ( ) - > g e t W o r l d O r i e n t a t i o n ( ) . I n v e r s e ( ) *  
                         ( m C a m e r a A c t o r - > _ g e t S c e n e N o d e ( ) - > g e t P a r e n t S c e n e N o d e ( ) - > g e t W o r l d P o s i t i o n ( )   -   g l o b a l C a m e r a P o s i t i o n ) ) ;  
  
  
                 m T a r g e t C a m e r a D i r e c t i o n   =   (   p a r t E y e s   -   m T a r g e t C a m e r a P o s i t i o n   ) . n o r m a l i s e d C o p y ( ) ;  
         }  
  
         f l o a t   D i a l o g C o n t r o l S t a t e : : g e t S h o w T e x t L e n g t h ( c o n s t   C e G u i S t r i n g &   t e x t )   c o n s t  
         {  
                 r e t u r n   0 . 0 1 9 f   *   t e x t . l e n g t h ( )   +   / /   Z e i t   f � r s   T e x t   l e s e n  
                               0 . 2 5 f ;                                       / /   F a d e   i n  
         }  
  
         v o i d   D i a l o g C o n t r o l S t a t e : : r e s p o n s e (  
                 A c t o r *   a c t o r ,   c o n s t   C e G u i S t r i n g &   t e x t ,   c o n s t   O g r e : : S t r i n g &   s o u n d F i l e )  
         {  
                 i f   (   a c t o r   = =   m D i a l o g P a r t n e r   )  
                         r e c a l c u l a t e C a m e r a (   m C h a r a c t e r A c t o r ,   m D i a l o g P a r t n e r   ) ;  
                 e l s e  
                         r e c a l c u l a t e C a m e r a (   m D i a l o g P a r t n e r ,   m C h a r a c t e r A c t o r   ) ;  
  
                 / /   U n g e f � h r e   L e s e d a u e r   b e s t i m m e n  
                 f l o a t   f a d e T i m e   =   g e t S h o w T e x t L e n g t h ( t e x t ) ;  
                 i f   ( s o u n d F i l e . l e n g t h ( )   = =   0 )  
                 {  
                         f l o a t   s p e e d   =   m S u b t i t l e S p e e d ;  
  
                         i f   (   m S u b t i t l e S p e e d   = =   0 . 0   )  
                                 s p e e d   =   1 . 0 ;  
  
                         m C u r r F a d e T e x t T i m e   =   f a d e T i m e * s p e e d ;  
                         m T o t a l F a d e T e x t T i m e   =   f a d e T i m e * s p e e d ;  
                 }  
                 e l s e  
                 {  
                         i f   (   m S o u n d O b j e c t   ! =   N U L L   )  
                                 m S o u n d O b j e c t - > g e t M o v a b l e O b j e c t ( ) - > g e t P a r e n t S c e n e N o d e ( ) - > d e t a c h O b j e c t (  
                                         m S o u n d O b j e c t - > g e t M o v a b l e O b j e c t ( )   ) ;  
  
                         d e l e t e   m S o u n d O b j e c t ;  
                         m S o u n d O b j e c t   =   n e w   S o u n d O b j e c t ( S o u n d M a n a g e r : : g e t S i n g l e t o n ( ) . g e t A c t i v e D r i v e r ( ) - >  
                                         c r e a t e S o u n d ( s o u n d F i l e ,   S T _ S A M P L E ) ,   s o u n d F i l e ) ;  
  
                         / /   A n   S p r e c h e r   h � n g e n  
                         a c t o r - > _ g e t S c e n e N o d e ( ) - > a t t a c h O b j e c t (   m S o u n d O b j e c t - > g e t M o v a b l e O b j e c t ( )   ) ;  
                         a c t o r - > _ g e t S c e n e N o d e ( ) - > _ u p d a t e (   t r u e ,   f a l s e   ) ;  
                         m S o u n d O b j e c t - > _ s e t A c t o r (   a c t o r   ) ;  
                         m S o u n d O b j e c t - > s e t 3 d ( t r u e ) ;  
                         m S o u n d O b j e c t - > p l a y ( ) ;  
                         m S o u n d O b j e c t - > _ u p d a t e ( ) ;  
  
  
                         m C u r r F a d e T e x t T i m e   =   s t d : : m a x ( f a d e T i m e * m S u b t i t l e S p e e d , m S o u n d O b j e c t - > g e t L e n g t h ( ) ) ;  
                         m T o t a l F a d e T e x t T i m e   =   m C u r r F a d e T e x t T i m e ;  
                 }  
  
                 M e s h O b j e c t *   m e s h   =   d y n a m i c _ c a s t < M e s h O b j e c t * > ( a c t o r - > g e t C o n t r o l l e d O b j e c t ( ) ) ;  
                 i f   ( m e s h   ! =   N U L L )  
                 {  
                         i f   ( m e s h - > h a s A n i m a t i o n ( " r e d e n " ) )  
                         {  
                                 m e s h - > s t o p A l l A n i m a t i o n s ( ) ;  
                                 m T a l k A n i m a t i o n   =   m e s h - > s t a r t A n i m a t i o n ( " r e d e n " ) ;  
                         }  
                 }  
  
                 L O G _ D E B U G ( L o g g e r : : U I ,  
                         " R e s p o n s e :   "  
                                 +   a c t o r - > g e t N a m e ( )  
                                 +   "   F i l e :   ' "  
                                 +   s o u n d F i l e  
                                 +   " ' ,   T e x t :   ' "   +   t e x t   +   " ' ,   T i m e :   "  
                                 +   S t r i n g C o n v e r t e r : : t o S t r i n g ( m C u r r F a d e T e x t T i m e ) ) ;  
  
  
                 m T e x t S h o w n   =   t r u e ;  
  
                 i f   ( m S u b t i t l e W i n d o w   ! =   N U L L   & &   ! t e x t . e m p t y ( ) )  
                 {  
                         m S u b t i t l e W i n d o w - > s h o w ( t e x t ) ;  
                 }  
         }  
  
         b o o l   D i a l o g C o n t r o l S t a t e : : m o u s e R e l e a s e d ( c o n s t   O I S : : M o u s e E v e n t &   e v t ,  
                 O I S : : M o u s e B u t t o n I D   i d )  
         {  
  
                 i f   ( m T e x t S h o w n   & &   ( m C u r r F a d e T e x t T i m e   +   0 . 2 5 )   <   m T o t a l F a d e T e x t T i m e )  
                 {  
                         m C u r r F a d e T e x t T i m e   =   - 1 ;  
  
                         r e t u r n   t r u e ;  
                 }  
                 e l s e  
                 {  
                         r e t u r n   f a l s e ;  
                 }  
         }  
  
 	 v o i d   D i a l o g C o n t r o l S t a t e : : s t a r t ( )  
 	 {  
 	 	 i f   ( ! m B o t   | |   ! m D i a l o g W i n d o w )  
 	 	 {  
 	 	 	 T h r o w ( I l l e g a l S t a t e E x c e p t i o n ,   " D i a l o g C o n t r o l S t a t e   n o t   p r o p e r l y   i n i t i a l i z e d . " ) ;  
 	 	 }  
  
 	 	 m D i a l o g W i n d o w - > s e t N a m e ( m B o t - > g e t N a m e ( ) ) ;  
 	 	 g e t R e s p o n s e ( D I A L O G _ S T A R T ) ;  
 	 }  
  
 	 v o i d   D i a l o g C o n t r o l S t a t e : : g e t R e s p o n s e ( c o n s t   C e G u i S t r i n g &   m s g )  
 	 {  
 	 	 d e l e t e   m C u r r e n t R e s p o n s e ;  
 	 	 m C u r r e n t R e s p o n s e   =   m B o t - > c r e a t e R e s p o n s e ( m s g ) ;  
 	 	  
 	 	 i f   ( m B o t - > h a s E x i t R e q u e s t ( ) )  
 	 	 {  
 	 	 	 m D i a l o g W i n d o w - > s e t D i a l o g E n d ( ) ;  
 	 	 	 m S t a t e   =   C L O S I N G _ D I A L O G ;  
 	 	 	 h a n d l e D i a l o g C l o s e ( ) ;  
 	 	 	 r e t u r n ;  
 	 	 }  
 	 	 i f   ( m C u r r e n t R e s p o n s e   = =   N U L L )  
 	 	 {  
 	 	 	 m D i a l o g W i n d o w - > s e t D i a l o g E n d ( ) ;  
 	 	 	 m S t a t e   =   C L O S I N G _ D I A L O G ;  
 	 	 / / 	 � b e r g a n g s l � s u n g ,   w e n n   g e r a d e   k e i n   s p r e c h e r   a k t i v   i s t   w i r d    
 	 	 / / 	 n i c h t   n i c h t   t e x t F i n i s h e d   a u f g e r u f e n  
 	 	 / /     h i e r   m � s s t e   w a h r s c h e i n l i c h   r e q u e s t C l o s e   a u f t a u c h e n  
 	 	 	 h a n d l e D i a l o g C l o s e ( ) ;  
 	 	 	 r e t u r n ;  
 	 	 }  
  
 	 	 D i a l o g R e s p o n s e : : R e s p o n s e s   r e s p o n s e s   =   m C u r r e n t R e s p o n s e - > g e t R e s p o n s e s ( ) ;  
 	 	 C e G u i S t r i n g   r e s p o n s e S o u n d   =   " n u l l . o g g " ;  
 	 	 C e G u i S t r i n g   r e s p o n s e T e x t ;  
 	 	 i f   ( ! r e s p o n s e s . e m p t y ( ) )  
 	 	 {  
 	 	 	 r e s p o n s e S o u n d   =   r e s p o n s e s . b e g i n ( ) - > f i r s t ;  
 	 	 	 r e s p o n s e T e x t   =   r e s p o n s e s . b e g i n ( ) - > s e c o n d ;  
 	 	 	 r e s p o n s e T e x t . c _ s t r ( ) ;  
 	 	 	 r e s p o n s e S o u n d . c _ s t r ( ) ; 	  
 	 	 }  
  
 	 	 r e s p o n s e ( m B o t - > g e t D i a l o g P a r t n e r ( ) - > g e t A c t o r ( ) ,    
 	 	 	 	 r e s p o n s e T e x t ,   r e s p o n s e S o u n d . c _ s t r ( ) ) ;  
  
 	 	 i f   ( ! r e s p o n s e T e x t . e m p t y ( ) )  
 	 	 {  
 	 	 	 m D i a l o g W i n d o w - > s e t R e s p o n s e ( r e s p o n s e T e x t ) ;  
  
 	 	 	 m G a m e L o g g e r - > l o g D i a l o g E v e n t ( m B o t - > g e t N a m e ( ) ,   r e s p o n s e T e x t ) ;  
 	 	 	 L O G _ M E S S A G E ( L o g g e r : : D I A L O G ,   m B o t - > g e t N a m e ( )   +   "   s a y s :   "   +   r e s p o n s e T e x t ) ;  
 	 	 }  
  
 	 	 m D i a l o g W i n d o w - > s e t V i s i b l e ( f a l s e ) ;  
 	 	 m S t a t e   =   T A L K I N G _ P A R T N E R _ C H A R A C T E R ;  
 	 	 m C u r r e n t R e s p o n s e T e x t   =   m s g ;  
 	 }  
  
 	 v o i d   D i a l o g C o n t r o l S t a t e : : t e x t F i n i s h e d ( )  
 	 {  
 	 	 i f   ( m S t a t e   = =   T A L K I N G _ P A R T N E R _ C H A R A C T E R )  
 	 	 {  
 	 	 	 g e t O p t i o n s ( m C u r r e n t R e s p o n s e T e x t ) ;  
 	 	 	 m D i a l o g W i n d o w - > s e t V i s i b l e ( t r u e ) ;  
 	 	 	 m S t a t e   =   C H O O S I N G _ O P T I O N ;  
 	 	 }  
 	 	  
 	 	 i f   ( m S t a t e   = =   T A L K I N G _ P L A Y E R _ C H A R A C T E R )  
 	 	 {  
 	 	 	 g e t R e s p o n s e ( m C u r r e n t R e s p o n s e T e x t ) ;  
 	 	 }  
 	 	  
 	 	 L O G _ D E B U G ( L o g g e r : : U I ,    
 	 	 	 	 	 S t r i n g C o n v e r t e r : : t o S t r i n g ( m S t a t e )  
 	 	 	 	 	 +   "   b e i   t e x t F i n i s h e d "   ) ;  
 	 }  
  
 	 v o i d   D i a l o g C o n t r o l S t a t e : : g e t O p t i o n s ( c o n s t   C e G u i S t r i n g &   q u e s t i o n )  
 	 {  
 	 	 i f   ( m C u r r e n t R e s p o n s e   = =   N U L L )  
 	 	 {  
 	 	 	 T h r o w ( r l : : I l l e g a l S t a t e E x c e p t i o n ,   " m C u r r e n t R e s p o n s e   m u s t   n o t   b e   N U L L . " ) ;  
 	 	 	 r e t u r n ;  
 	 	 }  
 	 	  
 	 	 D i a l o g R e s p o n s e : : D i a l o g O p t i o n s   o p t i o n s   =   m C u r r e n t R e s p o n s e - > g e t D i a l o g O p t i o n s ( ) ;  
 	 	  
 	 	 i f   ( o p t i o n s . e m p t y ( ) )  
 	 	 {  
 	 	 	 m D i a l o g W i n d o w - > s e t D i a l o g E n d ( ) ;  
 	 	 	 h a n d l e D i a l o g C l o s e ( ) ;  
 	 	 	 r e t u r n ;  
 	 	 }  
 	 	  
 	 	 / / m D i a l o g O p t i o n s - > c l e a r A l l S e l e c t i o n s ( ) ;  
  
 	 	 C e G u i S t r i n g V e c t o r   o p t i o n T e x t s ;  
 	 	 f o r ( D i a l o g R e s p o n s e : : D i a l o g O p t i o n s : : c o n s t _ i t e r a t o r   i t r   =   o p t i o n s . b e g i n ( ) ;    
 	 	 	 i t r   ! =   o p t i o n s . e n d ( ) ;   + + i t r )  
 	 	 { 	  
 	 	 	 C e G u i S t r i n g   c u r r e n t O p t i o n   =   ( * i t r ) - > g e t T e x t ( ) ;  
 	 	 	 L O G _ D E B U G ( L o g g e r : : U I ,   " P l a y e r   o p t i o n :   "   +   c u r r e n t O p t i o n ) ;  
 	 	 	  
 	 	 	 o p t i o n T e x t s . p u s h _ b a c k ( c u r r e n t O p t i o n ) ;  
 	 	 	 / / / @ t o d o   m a r k   a t t r i b u t e / t a l e n t   c h e c k s  
 	 	 }  
  
 	 	 m D i a l o g W i n d o w - > s e t A v a i l a b l e O p t i o n s ( o p t i o n T e x t s ) ;  
 	 }  
  
 	 b o o l   D i a l o g C o n t r o l S t a t e : : h a n d l e D i a l o g S e l e c t O p t i o n ( )  
 	 {  
 	 	 i n t   i d x   =   m D i a l o g W i n d o w - > g e t S e l e c t e d O p t i o n I n d e x ( ) ;  
 	 	 i f   ( i d x   ! =   - 1 )  
 	 	 {  
 	 	 	 D i a l o g O p t i o n *   o p t i o n   =   m C u r r e n t R e s p o n s e - > g e t D i a l o g O p t i o n s ( ) [ i d x ] ;  
 	 	 	 o p t i o n - > p r o c e s s S e l e c t i o n ( ) ;  
 	 	 	 m C u r r e n t R e s p o n s e T e x t   =   o p t i o n - > g e t P a t t e r n ( ) ;  
 	 	 	 C e G u i S t r i n g   s e l e c t e d O p t i o n   =   o p t i o n - > g e t T e x t ( ) ;  
 	 	 	 i f   ( m C u r r e n t R e s p o n s e T e x t   ! =   " 0 "   & &   m C u r r e n t R e s p o n s e T e x t   ! =   " 6 6 6 " )  
 	 	 	 {  
 	 	 	 	 i f   ( ! s e l e c t e d O p t i o n . e m p t y ( ) )  
 	 	 	 	 {  
 	 	 	 	 	 m S t a t e   =   T A L K I N G _ P L A Y E R _ C H A R A C T E R ;  
 	 	 	 	 	 m G a m e L o g g e r - > l o g D i a l o g E v e n t ( " H e l d " ,   s e l e c t e d O p t i o n ) ;  
 	 	 	 	 	 L O G _ M E S S A G E ( L o g g e r : : D I A L O G ,  
 	 	 	 	 	 	 " P l a y e r   s a y s :   "   +   s e l e c t e d O p t i o n ) ;  
 	 	 	 	 	 m D i a l o g W i n d o w - > s e t C h o s e n O p t i o n ( s e l e c t e d O p t i o n ) ; 	  
 	 	 	 	 	 r e s p o n s e (  
 	 	 	 	 	 	 m B o t - > g e t D i a l o g C h a r a c t e r ( ) - > g e t A c t o r ( ) ,    
 	 	 	 	 	 	 s e l e c t e d O p t i o n ,    
 	 	 	 	 	 	 o p t i o n - > g e t I d ( ) . c _ s t r ( ) ) ; 	 	 	  
 	 	 	 	 	 m D i a l o g W i n d o w - > s e t V i s i b l e ( f a l s e ) ;  
 	 	 	 	 }  
 	 	 	 }  
 	 	 }  
 	 	  
 	 	 r e t u r n   t r u e ;  
 	 }  
  
 	 b o o l   D i a l o g C o n t r o l S t a t e : : h a n d l e D i a l o g C l o s e ( )  
 	 {  
 	 	 I n p u t M a n a g e r : : g e t S i n g l e t o n ( ) . p o p C o n t r o l S t a t e ( ) ;  
 	 	 m D i a l o g W i n d o w - > s e t V i s i b l e ( f a l s e ,   t r u e ) ;  
 	 	 m S u b t i t l e W i n d o w - > s e t V i s i b l e ( f a l s e ,   t r u e ) ;  
 	 	 r e t u r n   t r u e ;  
 	 }  
  
 	 b o o l   D i a l o g C o n t r o l S t a t e : : r e q u e s t D i a l o g C l o s e ( )  
 	 {  
 	 / / 	 h a n d l e C l o s e   i s   c a l l e d   a u t o m a t i c a l l y    
 	 	 g e t R e s p o n s e ( D I A L O G _ E X I T ) ;  
 	 	 r e t u r n   t r u e ;  
 	 }  
  
  
 }  
 >>>>>>> .r3599
