package org.gradle;

import java.awt.Component;

import javax.swing.JMenuBar;

public class EditorMenuBar extends JMenuBar {

	public Component add(ObjectSelector o) {
		return add(o.button);
	}
	
}
