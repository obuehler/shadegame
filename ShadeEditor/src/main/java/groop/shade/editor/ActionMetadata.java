package groop.shade.editor;

public class ActionMetadata {
	public String type;
	public Float bearing;
	public Integer targetPixelX;
	public Integer targetPixelY;
	public Integer length;
	public Integer counter;
	public boolean cycleStart;
	
	/** Constructor for JSON parser */
	ActionMetadata() {
		type = null;
		bearing = null;
		targetPixelX = null;
		targetPixelY = null;
		length = null;
		counter = null;
		cycleStart = false;
	}
	
	ActionMetadata(String t, Float b, Integer l, Integer c, boolean cs, Integer tx, Integer ty) {
		type = t;
		bearing = b;
		length = l;
		counter = c;
		cycleStart = cs;
		targetPixelX = tx;
		targetPixelY = ty;
	}
}
