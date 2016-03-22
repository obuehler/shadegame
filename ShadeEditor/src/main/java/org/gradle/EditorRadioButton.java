package org.gradle;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JRadioButtonMenuItem;

public class EditorRadioButton extends JRadioButtonMenuItem {
	
	StageType type;
	ChangeableIcon icon;

	public EditorRadioButton(StageType stageType, ChangeableIcon changeableIcon) {
		super(stageType.getName());
		type = stageType;
		icon = changeableIcon;
		
		addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				icon.changeType(type);
				setSelected(true);
			}
			
		});
	}
	
}

