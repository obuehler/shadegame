package org.gradle;

public interface StageType {
	public abstract String getName();
	public abstract EditorItems getValue();
	public abstract String getIconPath();
}