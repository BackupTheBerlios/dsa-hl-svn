class Rock < GameObject
  def initialize(model, position, orientation, spawned)
    super("Fels", "Ein Fels")
    @mPosition = position
    @mOrientation = orientation
    @mModel = model
    @mSpawned = false

    if (spawned)
      spawn()
    end
  end

  def spawn
    if ( ! @mSpawned )
      rockActor = $AM.createMeshActor(getName(), @mModel, PhysicsManager::GT_ELLIPSOID, 2500.0)
      rockActor.getPhysicalThing().setGravityOverride(true)
      setActor(rockActor)

      rockActor.placeIntoScene( 
	@mPosition[0],
	@mPosition[1],
	@mPosition[2],
	@mOrientation[0],
	@mOrientation[1],
	@mOrientation[2],
	@mOrientation[3])

      @mSpawned = true
    end
  end

  def setGravityState(active)
    if (active)
      getActor().getPhysicalThing().setGravityOverride(true, 0.0, -9880.0, 0.0)
    else
      getActor().getPhysicalThing().setGravityOverride(true, 0.0, 0.0, 0.0)
    end
    getActor().getPhysicalThing().unfreeze();
  end
end

class RockManager
  def initialize
    @mRocks = Array.new()
  end

  def add(rock)
    @mRocks.push(rock)
  end

  def startFalling
    @mRocks.each { |rock| rock.setGravityState(true) }
  end

  def spawn
    @mRocks.each { |rock| rock.spawn() }
  end
end


class RockPile < GameObject
  def initialize(position, orientation)
    super("Steinhaufen", "Ein großer Steinhaufen")
    rockPile = $AM.createMeshActor("Steinhaufen", "Steinhaufen.mesh", PhysicsManager::GT_NONE, 25000.0)
    setActor(rockPile)
    rockPile.placeIntoScene( 
	position[0],
	position[1],
	position[2],
	orientation[0],
	orientation[1],
	orientation[2],
	orientation[3])
  end

  def collapse()
    getActor().getControlledObject().startAnimation("Zusammenfallen", 1.0, 1)
  end
end

# Zone für Questveränderung und später Steinschlag erstellen. Steinschlagauslösung ist abhängig davon ob das Quest "spinne" schon auf COMPLETED steht

print( "GameEvent-Tests wird geladen" );

print( "Definiere SteinschlagzoneListener" );
# Definition des GameAreaListeners
class SteinschlagzoneListener < GameAreaListener
	def initialize(rockPile)
		super()
		@mRockPile = rockPile
	end
	def areaLeft(anEvent)
		print( "Raus - " +  anEvent.getProvokingActor().getName() );
		$GameEveMgr.removeAreaListener(self)
	end
	def areaEntered(anEvent)
		print( "Rein - " + anEvent.getProvokingActor().getName() );
		RulesSubsystem.getSingleton().getQuestBook().getQuest("hoehleEingang").setState(Quest::COMPLETED)
		RulesSubsystem.getSingleton().getQuestBook().getQuest("hoehleZeug").setState(Quest::OPEN)
		RulesSubsystem.getSingleton().getQuestBook().getQuest("hoehleZeug").setState(Quest::OPEN)
		RulesSubsystem.getSingleton().getQuestBook().getQuest("hoehleZeugFackel").setState(Quest::OPEN)
		RulesSubsystem.getSingleton().getQuestBook().getQuest("hoehleZeugHebel").setState(Quest::OPEN)
		RulesSubsystem.getSingleton().getQuestBook().getQuest("hoehleZeugTrank").setState(Quest::OPEN)
		@mRockPile.collapse()
	end
end

