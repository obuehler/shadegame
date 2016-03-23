package org.gradle;

public interface ChangeableIcon {

	public void changeType(StageType e);
	
	/** Refreshes side menu when the type is changed */
	public void refreshMenu();
	
}