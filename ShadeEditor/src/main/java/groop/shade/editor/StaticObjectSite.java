package groop.shade.editor;

import com.badlogic.gdx.math.Vector2;

public class StaticObjectSite extends Vector2 {
	
	public StaticObjectType type;
	
	public double scale;
	
	public StaticObjectSite() {}
	
	public StaticObjectSite(Vector2 site, float s, StaticObjectType t) {
		super(site);
		scale = s;
		type = t;
	}

}
