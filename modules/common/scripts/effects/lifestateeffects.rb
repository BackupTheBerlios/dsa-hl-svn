class DeadEffect < Effect
  def initialize(stufe)
    super(0);
    setName("Tot.");
    setDescription("Das Leben ist unwideruflich vorbei.");
  end

  def getLifeState()
    return LS_DEAD;
  end
    
  def timeCheck()
    return PERMANENT;
  end
end

class IncapacitatedEffect < Effect
  def initialize(stufe)
    super(0);
    setName("Kampfunf�hig.");
    setDescription("Die Verletzungen sind zu stark um zu k�mpfen.");
  end

  def getLifeState()
    return LS_INCAPACITATED;
  end
    
  def timeCheck()
    return PERMANENT;
  end
end