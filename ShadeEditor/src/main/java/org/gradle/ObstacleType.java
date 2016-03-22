package org.gradle;

import java.nio.file.Paths;

import model.obstacles.Rock;
import model.obstacles.Tree;

public enum ObstacleType implements StageType {
	
	ROCK {			
		/* @SuppressWarnings("unchecked")
		@Override
		public Rock create(Vector2 camp, GameStage s) {
			Rock r = new Rock(camp);
			s.rocks.add(r);
			return r;
		}*/

		@Override
		public String getName() {
			return "Rock";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.ROCK;
		}

		@Override
		public String getIconPath() {
			return Paths.get(Constants.ASSETS_PATH() + Rock.IMAGE_PATH).toAbsolutePath().toString();
		}

	},
	
	TREE {
		/* @SuppressWarnings("unchecked")
		@Override
		 public Tree create(Vector2 camp, GameStage s) {
			Tree t = new Tree(camp);
			s.trees.add(t);
			return t;
		} */

		@Override
		public String getName() {
			return "Tree";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.TREE;
		}

		@Override
		public String getIconPath() {
			return Paths.get(Constants.ASSETS_PATH() + Tree.IMAGE_PATH).toAbsolutePath().toString();
		}

	};

	// public abstract <T extends Obstacle> T create(Vector2 camp, GameStage s);
	
}
