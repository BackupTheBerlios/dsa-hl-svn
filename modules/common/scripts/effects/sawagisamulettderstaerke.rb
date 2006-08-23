class SawagisAmulettDerStaerke < Effect
  def initialize(stufe)
    super(stufe);
    setName("Sawagis Amulett der Staerke");
    setDescription("Erh�ht die Koerperkraft.");
  end

  def apply()
	  getOwner().getEigenschaftenStateSet("KK").modifyModifier(+1);
  end

  def remove()
	  getOwner().getWertStateSet("KK").modifyModifier(-1);
  end
end
