require 'jobs/jobsequence.rb'
require 'jobs/soundjobs.rb'
require 'jobs/animationjobs.rb'
require 'jobs/closurejobs.rb'

class JobSequenceTestAction < Action
  def initialize
    super("sequence", "Test job sequence.");
  end
  
  def canDo(go, user)
    true
  end
  
  def doAction(go, user, target)
    jobSequence(
            [
                _PlayAnimation(go.getActor(), "auf"),
                _PlaySound3d("doorcreak.ogg", go.getPosition()),
                2,
                _PlaySound3d("lachen.ogg", go.getPosition()),
                2,
                _PlayAnimation(go.getActor(), "zu"),
                _PlaySound3d("doorcreak.ogg", go.getPosition()),
				2,
				_Execute {provokeEror},
				2,
				_Execute {p "FOO"}
            ]
    );
  end
end

class JobQueueTestAction < Action
  def initialize
    super("queue", "Test job queue");
  end
  
  def canDo(go, user)
    true
  end
  
  def doAction(go, user, target)
    jobQueue(
            [
                [
					_PlayAnimation(go.getActor(), "auf"), 
					_PlaySound3d("doorcreak.ogg", go.getPosition())
				],
				_PlaySound3d("lachen.ogg", go.getPosition()),
				[
					_PlayAnimation(go.getActor(), "zu"), 
					_PlaySound3d("doorcreak.ogg", go.getPosition())
				],
				2,
				_Execute {provokeEror},
				_Execute {p "FOO"}
            ]
    )
  end
end

class GameTimeJobQueueTestAction < Action
  def initialize
    super("gametqueue", "Test game time job queue");
  end
  
  def canDo(go, user)
    true
  end
  
  def doAction(go, user, target)
    jobQueue(
            [
                [
					_PlayAnimation(go.getActor(), "auf"), 
					_PlaySound3d("doorcreak.ogg", go.getPosition())
				],
				Date.new(Date::ONE_KAMPFRUNDE),
				_PlaySound3d("lachen.ogg", go.getPosition()),
				[
					_PlayAnimation(go.getActor(), "zu"), 
					_PlaySound3d("doorcreak.ogg", go.getPosition())
				],
				Date.new(Date::ONE_SPIELRUNDE),
				_PlaySound3d("lachen.ogg", go.getPosition()),
				_Execute {provokeEror},
				_Execute {p "FOO"}
            ]
    )
  end
end

class JobSequenceTest < TestCase
    def execute()
        # Define a base box to place everything else on.
        height = 0.3
        min_base = [-2.0, 0.05, -2.0]
        max_base = [2.0, height, 2.0]
        base = $AM.createBoxPrimitiveActor(
             "JobSequenceTestBase", min_base, max_base, "alpha_green")
        base.placeIntoScene(getCenter());

        chest = $GOM.createGameObject(
            "JobSequenceTestChest");
        chest.addAction(JobSequenceTestAction.new());
        chest.addAction(JobQueueTestAction.new()); 
        chest.addAction(GameTimeJobQueueTestAction.new());
		chest.placeIntoScene();
        chest.setPosition(rel_pos([0.0, height, 0.0]));

        $SCRIPT.log("JobSequenceTest initialisiert");
    end
end

