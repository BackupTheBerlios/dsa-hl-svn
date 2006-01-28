package meshhandle.data;

import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;

public class Vector3 {
    private float mX, mY, mZ;

    public Vector3(float x, float y, float z) {
        mX = x;
        mY = y;
        mZ = z;
    }

    public float getX() {
        return mX;
    }

    public void setX(float x) {
        mX = x;
    }

    public float getY() {
        return mY;
    }

    public void setY(float y) {
        mY = y;
    }

    public float getZ() {
        return mZ;
    }

    public void setZ(float z) {
        mZ = z;
    }

    public void scale(float factor) {
        mX *= factor;
        mY *= factor;
        mZ *= factor;
    }

    public static Vector3 createFromXML(Node node) {
        NamedNodeMap attrs = node.getAttributes();
        return new Vector3(Float.parseFloat(attrs.getNamedItem("x")
                .getNodeValue()), Float.parseFloat(attrs.getNamedItem("y")
                .getNodeValue()), Float.parseFloat(attrs.getNamedItem("z")
                .getNodeValue()));
    }

    public String toXML(String nodeName) {
        return "<" + nodeName + " x=\"" + round(mX) + "\"" + " y=\""
                + round(mY) + "\"" + " z=\"" + round(mZ) + "\" />";
    }

    private float round(float y) {
        return Math.abs(y) < 1E-5 ? 0 : y;
    }

}
