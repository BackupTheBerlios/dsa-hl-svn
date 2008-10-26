#################################################
 # Copyright (C) 2008  Stefan Stammberger
 #
 # This library is free software; you can redistribute it and/or
 # modify it under the terms of the GNU Lesser General Public
 # License as published by the Free Software Foundation; either
 # version 2.1 of the License, or (at your option) any later version.
 #
 # This library is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 # Lesser General Public License for more details.
 #
 # You should have received a copy of the GNU Lesser General Public
 # License along with this library; if not, write to the Free Software
 # Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 #################################################

import sys
import xml.dom.minidom as xml

import ctypes
import ogre.renderer.OGRE as og

from MovePivot import *
from GameObjectClassManager import *
from MyRaySceneQueryListener import *

class ModuleManager(object):
    def __init__(self,  ogreRoot,  sceneManager):
        self.sceneManager = sceneManager
        self.ogreRoot = ogreRoot
        self.raySceneQuery = self.sceneManager.createRayQuery(og.Ray())

        self.gocManager = GameObjectClassManager()

        self.userSelectionList = []
        self.cutList = [] # selection objects that has been cut out and wait to be pasted again
        self.cutListPreviousNodes = [] # contains the nodes they where copnnected to before the cut

        self.listenerDings = MyRaySceneQueryListener()

        self.lastRay = None
#        self.rayLine = None

        self.pivot = None
        self.movingPivot = False

        self.leftMouseDown = False
        self.middleMouseDown = False
        self.rightMouseDown = False

        self.dropCount = 0
        self.dropNode = None
        self.dropEntity = None
        self.dropCollisionPlane = og.Plane(og.Vector3.UNIT_Y, og.Vector3.ZERO)

        self.numerOfCopys = 0 #everytime a copy is made this numer is increased to generate unique node and mesh names

    def load(self,  moduleName,  mapFiles,  gofFiles):
        self.moduleName = moduleName
        self.mapFiles = mapFiles
        self.gofFiles = gofFiles

        self.gocManager.parseGOFFiles(self.gofFiles)

        for a in self.mapFiles:
            doc = xml.parse(a)
            node = doc.getElementsByTagName("entity")
            if node != None:
                self.parseSceneNodes(node)

        self.pivot = Pivot(self.sceneManager)
        self.pivot.hide()

    def parseSceneNodes(self,  nodeList):
        for ent in nodeList:
            entityName = ent.attributes["name"].nodeValue # get the name of the ent
            meshFile = ent.attributes["meshfile"].nodeValue # meshfile
            nodePosition = None
            nodeRotation = None
            nodeScale = None

            for cn in ent.childNodes:
                if cn.nodeType == cn.ELEMENT_NODE:
                    if cn.localName == "position":
                        px = float(cn.attributes["x"].nodeValue)
                        py = float(cn.attributes["y"].nodeValue)
                        pz = float(cn.attributes["z"].nodeValue)
                        nodePosition = og.Vector3(px, py, pz)
                        continue

                    if cn.localName == "rotation":
                        qw = float(cn.attributes["qw"].nodeValue)
                        qx = float(cn.attributes["qx"].nodeValue)
                        qy = float(cn.attributes["qy"].nodeValue)
                        qz = float(cn.attributes["qz"].nodeValue)
                        nodeRotation = og.Quaternion(qw,  qx, qy, qz)
                        continue

                    if cn.localName == "scale":
                        px = float(cn.attributes["x"].nodeValue)
                        py = float(cn.attributes["y"].nodeValue)
                        pz = float(cn.attributes["z"].nodeValue)
                        nodeScale = og.Vector3(px, py, pz)
                        continue

            try:
                e = self.sceneManager.createEntity(entityName, meshFile)
            except:
                print "Warning: Meshfile " + meshFile + " could not be found."
                return

            n = self.sceneManager.getRootSceneNode().createChild(entityName + "_node")
            n.attachObject(e)
            n.setPosition(nodePosition)
            #n.setOrientation(nodeRotation)
            n.setScale(nodeScale)

        pass

    # called when a click into Main Ogre Window occurs
    def selectionClick(self,  ray,  controlDown=False,  shiftDown=False):
        self.listenerDings.reset()
        self.lastRay = ray
        self.listenerDings.currentRay = ray
        self.raySceneQuery.Ray = ray
        self.raySceneQuery.execute(self.listenerDings)

        so = self.listenerDings.rayCastToPolygonLevel(ray)
        if so is not None:
            if not so.isPivot:
                if self.pivot is not None:
                    self.pivot.show()
                if not controlDown and not shiftDown:
                    self.resetSelection()
                    so.setSelected(True)
                    self.userSelectionList.append(so)
                    self.updatePivots()
                elif controlDown and not shiftDown:
                    so.setSelected(True)

                    for soFromList in self.userSelectionList:
                        if soFromList == so:
                            return # object already selected

                    self.userSelectionList.append(so)
                    self.updatePivots()


                elif not controlDown and shiftDown:
                    for selo in self.userSelectionList:
                        if so == selo:
                            so.setSelected(False)
                            self.userSelectionList.remove(selo)
                    self.updatePivots()
            else:
                #so.entity is the pivot direction that was clicked
                self.pivot.startTransforming(so.entity,  self.userSelectionList)
        else:
            self.resetSelection() # click in empty space, deselect everything
            if self.pivot is not None:
                self.pivot.hide()

#        if self.rayLine == None:
#            self.rayLine = self.sceneManager.createManualObject("rayLine")
#            self.rayLine.setDynamic(True)
#            self.sceneManager.getRootSceneNode().createChildSceneNode("raynode").attachObject(self.rayLine)
#
#            self.rayLine.begin("BaseWhiteNoLighting", og.RenderOperation.OT_LINE_STRIP)
#
#            self.rayLine.position(ray.getOrigin())
#            self.rayLine.position( ray.getPoint(10000))
#
#            self.rayLine.end()
#
#        else:
#            self.rayLine.beginUpdate(0)
#
#            self.rayLine.position(ray.getOrigin())
#            self.rayLine.position( ray.getPoint(10000))
#
#            self.rayLine.end()

    def deleteObjects(self):
        if len(self.userSelectionList) < 1:
            return

        self.pivot.hide()

        for so in self.userSelectionList:
            self.sceneManager.destroySceneNode(so.entity.getParentNode().getName())
            del so

        self.userSelectionList = []

    def incrementNameSuffixNumber(self, name):
        newName = ""
        split = name.split("_")
        lastPart = len(split)-1
        newName = name.rstrip(split[lastPart])
        newName = newName + str(self.numerOfCopys)

#        if split[lastPart].isdigit() and not split[lastPart].startswith("0"):
#            num = int(split[lastPart])
#            num = num + 1
#            newName = name.rstrip(split[lastPart])
#            newName = newName + str(num)
#        else:
#            newName = name + "_1"

        self.numerOfCopys = self.numerOfCopys + 1
        return newName

    def copyObjects(self):
        if len(self.userSelectionList) < 1:
            return

        newSelectionList = []
        print "dbg: "
        for so in self.userSelectionList:
            nodeName = self.incrementNameSuffixNumber(so.entity.getParentNode().getName())
            newNode = self.sceneManager.getRootSceneNode().createChild(nodeName)

            entityName = self.incrementNameSuffixNumber(so.entity.getName())
            newEntity = self.sceneManager.createEntity(entityName, so.entity.getMesh().getName())

            newNode.attachObject(newEntity)
            newNode.setPosition(so.entity.getParentNode().getPosition())
            newNode.setOrientation(so.entity.getParentNode().getOrientation())
            newNode.setScale(so.entity.getParentNode().getScale())

            newSO = SelectionObject(newEntity, so.distance)
            newSO.setSelected(True)
            newSelectionList.append(newSO)

        self.resetSelection()
        self.userSelectionList = newSelectionList

    def cutObjects(self):
        if len(self.userSelectionList) < 1:
            return

        self.cutList = []
        for so in self.userSelectionList:
            self.cutListPreviousNodes.append(so.entity.getParentNode().getParent())
            so.entity.getParentNode().getParent().removeChild(so.entity.getParentNode())
            # set the "point of gravity" of all the cutted nodes to world origin at 0,0,0
            # so we only have to translate them to their new destination when they get pasted
            # the position of the pivot point is considered as the center of gravity
            so.entity.getParentNode().setPosition(so.entity.getParentNode().getPosition() - self.pivot.getPosition())
            self.cutList.append(so)
        self.resetSelection()
    def pasteObjects(self,  ray):
        if len(self.cutList) < 1:
            return

        result = og.Math.intersects(ray, self.dropCollisionPlane)
        if result.first == True:
            i=0
            while i < len(self.cutList):
                self.cutListPreviousNodes[i].addChild(self.cutList[i].entity.getParentNode())
                self.cutList[i].entity.getParentNode().translate(ray.getPoint(result.second))
                i = i+1
        else:
            i=0
            while i < len(self.cutList):
                self.cutListPreviousNodes[i].addChild(self.cutList[i].entity.getParentNode())
                self.cutList[i].entity.getParentNode().translate(ray.getPoint(100.0))
                i = i+1
        self.cutList = []

    def leftMouseUp(self):
        if self.pivot is not None and self.pivot.isTransforming:
            self.pivot.stopTransforming()

#    def iterateEntityUnderMouse(self):
#        self.listenerDings.iterateEntityUnderMouse()
#
#        pass

    def resetSelection(self):
        for so in self.userSelectionList:
            so.setSelected(False)

        self.userSelectionList = []

        self.listenerDings.reset()
        pass


    def updatePivots(self):
        newPivotPosition = og.Vector3(0, 0, 0)

        for so in self.userSelectionList:
            newPivotPosition += so.entity.getParentNode().getPosition()
        if self.pivot is not None:
            self.pivot.setPosition(newPivotPosition / len(self.userSelectionList))

    def unload(self,  saveOnUnload=True):
        pass

    def save(self):
        pass

    def startDropModelAction(self, meshFile, ray):
        self.dropEntity = self.sceneManager.createEntity("dropMesh" + str(self.dropCount), str(meshFile))
        self.dropNode = self.sceneManager.getRootSceneNode().createChild("dropNode" + str(self.dropCount))
        self.dropNode.attachObject(self.dropEntity)

        result = og.Math.intersects(ray, self.dropCollisionPlane)
        if result.first == True:
            self.dropNode.setPosition(ray.getPoint(result.second))
        else:
            self.dropNode.setPosition(ray.getPoint(50))

        self.dropCount += 1

    def moveDropModelAction(self, ray):
        result = og.Math.intersects(ray, self.dropCollisionPlane)
        if result.first == True:
            self.dropNode.setPosition(ray.getPoint(result.second))
        else:
            self.dropNode.setPosition(ray.getPoint(50))


    def stopDropModelAction(self, ray):
        pass

