package groop.shade.editor;

import java.awt.Component;

import javax.swing.JMenuBar;

@SuppressWarnings("serial")
public class EditorMenuBar extends JMenuBar {

	public Component add(ObjectSelector o) {
		return add(o.button);
	}

}
