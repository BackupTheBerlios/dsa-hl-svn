include RlScript

class MapTestModule < ContentModule
    def initialize()
        super("Maptest", "MapTest", false, 200608012)
    end

    def getDependencies()
        return ["common"]
    end

    def getTextureLocations()
	return ["textures"]
    end

    def getModelLocations()
	return ["Model"]
    end

    def getSoundLocations()
	return ["Sound"]
    end

    def start()
        $CORE.loadMap("Octree", "maptest.scene", "Maptest", "maptest.rb");
        $World = $CORE.getWorld();
    end
end

CoreSubsystem.getSingleton().registerModule(MapTestModule.new())
