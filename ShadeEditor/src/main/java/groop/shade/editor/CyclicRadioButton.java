package groop.shade.editor;

import javax.swing.JRadioButton;

public class CyclicRadioButton extends JRadioButton {
	ActionMetadata data;
	
	CyclicRadioButton(ActionMetadata d) {
		data = d;
	}
}
