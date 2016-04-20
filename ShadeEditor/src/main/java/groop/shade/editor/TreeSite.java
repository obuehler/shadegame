package groop.shade.editor;

import java.util.ArrayList;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public class TreeSite extends Vector2 {
	
	public ObstacleType type;
	
	public double scale;
	
	/** The actual action queue */
	public ArrayList<ActionMetadata> actionQueue;
	
	/** The list of available actions */
	public String[] actions;

	/** Empty constructor for JSON parsing */
	public TreeSite() {}

	public TreeSite(Vector2 site, float s, ArrayList<ActionMetadata> queue, ObstacleType t) {
		super(site);
		scale = s;
		actionQueue = queue;
		actions = null;
		type = t;
		
		switch (type) {
		case PEDESTRIAN:
			actions = Constants.PEDESTRIAN_ACTIONS;
			break;
		case CAR:
			actions = Constants.CAR_ACTIONS;
			break;
		default:
			break;
		}
		
	}
}
