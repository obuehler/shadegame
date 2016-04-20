package groop.shade.editor;

public class ActionMetadata {
	public String type;
	public float bearing;
	public int length;
	public int counter;
	public boolean cycleStart;
	
	ActionMetadata(String t, float b, int l, int c, boolean cs) {
		type = t;
		bearing = b;
		length = l;
		counter = c;
		cycleStart = cs;
	}
}
