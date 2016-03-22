package org.gradle;

import model.obstacles.Tree;

import com.badlogic.gdx.math.Vector2;

public class TreeSite extends Vector2 {
	
	/** Empty constructor for JSON parsing */
	public TreeSite() {}
	
	public TreeSite(Vector2 site) {
		super(site);
	}
	
	public Tree createTree() {
		return new Tree(this);
	}

}
