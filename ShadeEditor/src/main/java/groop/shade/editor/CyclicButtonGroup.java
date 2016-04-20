package groop.shade.editor;

import javax.swing.ButtonGroup;

public class CyclicButtonGroup extends ButtonGroup {
	CyclicRadioButton beginningOfCycle;
	
	CyclicButtonGroup() {
		super();
		beginningOfCycle = null;
	}
}
