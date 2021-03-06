 #################################################
# This source file is part of Rastullahs Lockenwickler.
# Copyright (C) 2003-2009 Team Pantheon. http://www.team-pantheon.de
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US
 #################################################

import ctypes as ctypes
import random
import platform

import ogre.renderer.OGRE as og

# get the light out of a light node
def extractLight(node):
        i = 0
        num = node.numAttachedObjects()
        while i < node.numAttachedObjects():
            c = node.getAttachedObject(i)
            tp = str(type(c))
            if tp == "<class 'ogre.renderer.OGRE._ogre_.Light'>":
                return c
            i += 1
            
# get the light out of a light node
def extractEntity(node):
        i = 0
        num = node.numAttachedObjects()
        while i < node.numAttachedObjects():
            c = node.getAttachedObject(i)
            tp = str(type(c))
            if tp == "<class 'ogre.renderer.OGRE._ogre_.Entity'>":
                return c
            i += 1

# a class to store information about a object that got selected
class SelectionObject():
    def __init__(self,  entity):
        self.entityName = entity.getName()
        self.entity = entity #the selected entity
        self.isPivot = False

#        print self.entity.getParentNode().getName()

    #if True this instance will show its bounding box else it will hide it
    def setSelected(self,  selected):
        if selected == True:
            self.entity.getParentNode().showBoundingBox(True)
        else:
            self.entity.getParentNode().showBoundingBox(False)

    def __eq__(self, other):
        return self.entity.getName() == other.entity.getName()

    def __ne__(self, other):
        return self.entity.getName() != other.entity.getName()

# class to handle material switching without having to modify scene materials individually
class MaterialSwitcher( og.MaterialManager.Listener ):
    def __init__(self):
        og.MaterialManager.Listener.__init__(self)
      
        self.currentColor = og.ColourValue(0.0, 0.0, 0.0)
        self.currentColorAsVector3 = og.Vector3()

        self.lastEntity = ""
        self.lastTechnique = None
 

        if platform.system() == "Windows":
            self.lastTechnique = og.MaterialManager.getSingleton().load("PlainColor", og.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME).getTechnique(0)
        else:
            self.lastTechnique = og.MaterialManager.getSingleton().load("PlainColorGLSL", og.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME).getTechnique(0)
        
        self.colorDict = {}
       
    # takes into account that one Entity can have multiple SubEntities
    def handleSchemeNotFound(self, index, name, material, lod, subEntity):

        temp = str(type(subEntity))
        if temp == "<class 'ogre.renderer.OGRE._ogre_.SubEntity'>":
            if self.lastEntity == subEntity.getParent().getName():
                subEntity.setCustomParameter(1, og.Vector4(self.currentColor.r, self.currentColor.g, self.currentColor.b, 1.0))
                #print str(subEntity.getParent().getRenderQueueGroup())
                return self.lastTechnique
            else:
                self.randomizeColor()
                subEntity.setCustomParameter(1, og.Vector4(self.currentColor.r, self.currentColor.g, self.currentColor.b, 1.0))
                
                self.lastEntity = subEntity.getParent().getName()
                self.colorDict[self.lastEntity] = self.currentColorAsVector3
                return self.lastTechnique

    def randomizeColor(self):
        r = random.randrange(1, 255)
        g = random.randrange(1, 255)
        b = random.randrange(1, 255)
        self.currentColorAsVector3 = og.Vector3(r, g, b)
        var = 1.0 / 255.0

        self.currentColor = og.ColourValue(r * var, g * var, b * var)

        #print str(int(self.currentColor.r * 255)) + " " + str(int(255 * self.currentColor.g)) + " " + str(int(255 * self.currentColor.b))
    
    def reset(self):
        self.currentColor = og.ColourValue(0.0, 0.0, 0.0)
        self.lastEntity = ""
        
# We need this attached to the depth target, otherwise we get problems with the compositor
# MaterialManager.Listener should NOT be running all the time - rather only when we're
# specifically rendering the target that needs it
class SelectionRenderListener(og.RenderTargetListener):
    def __init__(self, materialListener):
        og.RenderTargetListener.__init__(self)
        self.materialListener = materialListener
 
    def preRenderTargetUpdate(self, evt):
        og.MaterialManager.getSingleton().addListener( self.materialListener )
 
    def postRenderTargetUpdate(self, evt):
        og.MaterialManager.getSingleton().removeListener( self.materialListener )

        
class SelectionBuffer():
    def __init__(self, sceneManager,  renderTarget, moduleManager, zoneManager):
        self.sceneMgr = sceneManager
        self.camera = sceneManager.getCamera("MainCam")
        self.moduleManager = moduleManager
        self.zoneManager = zoneManager
        
        self.renderTarget = renderTarget
        
        # This is the material listener - Note: it is controlled by a seperate
        # RenderTargetListener, not applied globally to all targets
        self.materialSwitchListener = MaterialSwitcher()
        
        self.selectionTargetListener = SelectionRenderListener( self.materialSwitchListener )
        
        width = self.renderTarget.getWidth()
        height = self.renderTarget.getHeight()
        
        self.texture = og.TextureManager.getSingleton().createManual("SelectionPassTex", 
                                                                    og.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, 
                                                                    og.TEX_TYPE_2D, 
                                                                    width, 
                                                                    height, 
                                                                    0, og.PixelFormat.PF_R8G8B8, og.TU_RENDERTARGET)
                                                                    
        self.renderTexture = self.texture.getBuffer().getRenderTarget()
        self.renderTexture.setAutoUpdated(False)
        self.renderTexture.setPriority(0)                                                         
        self.renderTexture.addViewport( self.camera )
        self.renderTexture.getViewport(0).setOverlaysEnabled(False)
        self.renderTexture.getViewport(0).setClearEveryFrame(True)
        self.renderTexture.addListener( self.selectionTargetListener )
        self.renderTexture.getViewport(0).setMaterialScheme("aa")
        
#        self.createRTTOverlays()

    def update(self):
        self.updateBufferSize()
        
        self.renderTexture.update()        
        self.materialSwitchListener.reset()
        
        pixelBuffer = self.texture.getBuffer()
        bufferSize = pixelBuffer.getSizeInBytes()
        #buffersize2 = self.renderTexture.getWidth()*self.renderTexture.getHeight()*4
        
        storageclass = ctypes.c_uint8 * (bufferSize)
        self.buffer = storageclass()
        
        VoidPointer = og.CastVoidPtr(ctypes.addressof(self.buffer))

        self.pBox = og.PixelBox(pixelBuffer.getWidth(), pixelBuffer.getHeight(),pixelBuffer.getDepth(), pixelBuffer.getFormat(), VoidPointer)
        self.renderTexture.copyContentsToMemory(self.pBox, og.RenderTarget.FrameBuffer.FB_FRONT)
        
#        self.renderTexture.writeContentsToFile("selectionbuffer.png")
#        i = 0
#        
#        while i < len(self.buffer):
#            #print str(self.buffer[i + 2]) + " " + str(self.buffer[i+1]) + " " + str(self.buffer[i])
#            
#            i += 4
    
    def updateBufferSize(self):
        width = self.renderTarget.getWidth()
        height = self.renderTarget.getHeight()
        needsSizeUpdate = False
        
        if width is not self.renderTexture.getWidth():
            needsSizeUpdate = True
        if height is not self.renderTexture.getHeight():
            needsSizeUpdate = True            
        
        if needsSizeUpdate:
            og.TextureManager.getSingleton().unload("SelectionPassTex")
            
            self.texture = og.TextureManager.getSingleton().createManual("SelectionPassTex", 
                                                            og.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, 
                                                            og.TEX_TYPE_2D, 
                                                            width, 
                                                            height, 
                                                            0, og.PixelFormat.PF_R8G8B8, og.TU_RENDERTARGET)
                                                                    
            self.renderTexture = self.texture.getBuffer().getRenderTarget()
            self.renderTexture.setAutoUpdated(False)
            self.renderTexture.setPriority(0)                                                         
            self.renderTexture.addViewport( self.camera )
            self.renderTexture.getViewport(0).setOverlaysEnabled(False)
            self.renderTexture.getViewport(0).setClearEveryFrame(True)
            self.renderTexture.addListener( self.selectionTargetListener )
            self.renderTexture.getViewport(0).setMaterialScheme("aa")
        else:
            return
            
    def onSelectionClick(self, x, y):
        self.update()
        
        posInStream = (self.pBox.getWidth() * y - 1)*4
        posInStream += x*4
        
        colVec = og.Vector3(self.buffer[posInStream + 2], self.buffer[posInStream+1], self.buffer[posInStream])
        
        for key in self.materialSwitchListener.colorDict:
            if self.materialSwitchListener.colorDict[key] == colVec:
                if key == "OgreMainWin::0::ViewportGrid":
                    return None
                elif key == "rayLine":
                    return None
                elif key == "TerrainManagerEditingCircle" or  key == "TerrainManagerPointerNodeMesh":
                    return None
                elif key == "EditorXArrow":
                    so = SelectionObject(self.sceneMgr.getEntity(key))
                    so.isPivot = True
                    return so
                elif key == "EditorYArrow":
                    so = SelectionObject(self.sceneMgr.getEntity(key))
                    so.isPivot = True
                    return so
                elif key == "EditorZArrow":
                    so = SelectionObject(self.sceneMgr.getEntity(key))
                    so.isPivot = True
                    return so
                elif key == "EditorXRotator" or key == "EditorYRotator" or key == "EditorZRotator":
                    so = SelectionObject(self.sceneMgr.getEntity(key))
                    so.isPivot = True
                    return so
                elif key == "EditorXScaler" or key == "EditorYScaler" or key == "EditorZScaler" or key == "UniScaler":
                    so = SelectionObject(self.sceneMgr.getEntity(key))
                    so.isPivot = True
                    return so
                else:
                    so = SelectionObject(self.sceneMgr.getEntity(key))
                    return so

        return None
        
    def manualSelectObjects(self, itemNodes):
        items = []
        for key in itemNodes:
            parentNode = None
            if key.startswith("Map: "):
                parentNode = self.moduleManager.mainModule.getMap(key.replace("Map: ", "")).mapNode
                for nodeName in itemNodes[key]:                    
                    n = parentNode.getChild(nodeName).getAttachedObject(0)
                    if parentNode.getChild(nodeName).getName().startswith("light_"):
                        n = extractEntity(parentNode.getChild(nodeName))
                        
                    so = SelectionObject(n)
                    so.setSelected(True)
                    items.append(so)
            elif key.startswith("Zone: "):
                zone = self.zoneManager.getZone(key.replace("Zone: ", ""))
                if zone is None:
                    continue
                    
                parentNode = zone.zoneNode
                for nodeName in itemNodes[key]:
                    obj = parentNode.getChild(nodeName).getAttachedObject(0)
                    so = SelectionObject(obj)
                    so.setSelected(True)
                    items.append(so)
        
        return items
        
        
    def createRTTOverlays(self):
        baseWhite = og.MaterialManager.getSingletonPtr().getByName("Lockenwickler_Pivot_X")
        SelectionBufferTexture = baseWhite.clone("SelectionDebugMaterial")
        textureUnit = SelectionBufferTexture.getTechnique(0).getPass(0).createTextureUnitState()
 
        textureUnit.setTextureName("SelectionPassTex")
 
 
        overlayManager = og.OverlayManager.getSingleton()
        # Create an overlay
        self.mDebugOverlay = overlayManager.create("OverlayName")
 
        # Create a panel
        panel = overlayManager.createOverlayElement("Panel", "PanelName0")
        panel.setMetricsMode(og.GMM_PIXELS)
        panel.setPosition(10, 10)
        panel.setDimensions(400, 280)
        panel.setMaterialName("SelectionDebugMaterial") 
        self.mDebugOverlay.add2D(panel)
 
        self.mDebugOverlay.show()



