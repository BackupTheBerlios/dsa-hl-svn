require 'testcase.rb'
require 'npc.rb'

class NpcTest < TestCase
    def execute()
        
        height = 0.3
        min_base = [-3.0, 0.05, -3.0]
        max_base = [3.0, height, 3.0]
        base = $AM.createBoxPrimitiveActor("DialogTestBase",
                                           min_base, max_base, "alpha_blue")
        base.placeIntoScene(getCenter());

        npc1proxy = $GOM.createGameObject("TestPerson");
        npc1 = npc1proxy;
        npc1.setPosition(rel_pos([0.0, height, 0.0]));

        npc1.placeIntoScene();
    end
end
