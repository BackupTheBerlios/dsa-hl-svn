include RlScript

class AITestModule < ContentModule
    def initialize()
        super("aitest", "aitest", false, 20070303)
    end

    def getDependencies()
        return ["common"]
    end

    def getTextureLocations()
        return ["textures"]
    end

    def getModelLocations()
        return []
    end

    def getSoundLocations()
        return []
    end

    def start()
        $CORE.loadMap("Octree", "aitest.scene", "aitest", "aitest.rb")
        $World = $CORE.getWorld();
    end
end

CoreSubsystem.getSingleton().registerModule(AITestModule.new())
