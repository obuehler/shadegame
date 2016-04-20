package groop.shade.editor;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JLabel;

@SuppressWarnings("serial")
public class ActionMetadataPanel extends JPanel {
	
	private static class CycleResetListener implements ActionListener {

		CyclicButtonGroup cbg;
		static CycleResetListener instance = null;
		
		private CycleResetListener(CyclicButtonGroup c) {
			cbg = c;
		}
		
		@Override
		public void actionPerformed(ActionEvent arg0) {
			if (cbg.beginningOfCycle != null) cbg.beginningOfCycle.data.cycleStart = false;
			cbg.beginningOfCycle = (CyclicRadioButton) arg0.getSource(); 
			cbg.beginningOfCycle.data.cycleStart = true;
		}
		
		static CycleResetListener getInstance(CyclicButtonGroup c) {
			if (instance == null) {
				instance = new CycleResetListener(c);
			}
			return instance;
		}
		
	}
	
	ActionMetadataPanel(final ActionMetadata data, final CyclicButtonGroup cyclicButtons, final ArrayList<ActionMetadata> actions, final AttributeMenu menu) {
		
		setLayout(new BoxLayout(this, BoxLayout.LINE_AXIS));
		
		JPanel infoPanel = new JPanel();
		JPanel buttonPanel = new JPanel();
		infoPanel.setLayout(new BoxLayout(infoPanel, BoxLayout.PAGE_AXIS));
		buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.PAGE_AXIS));
		
		infoPanel.add(new JLabel(data.type));
		infoPanel.add(new JLabel("length: " + Integer.toString(data.length)));
		infoPanel.add(new JLabel("counter: " + Integer.toString(data.counter)));
		infoPanel.add(new JLabel("bearing: " + Float.toString(data.bearing)));
		
		final CyclicRadioButton cyclicButton = new CyclicRadioButton(data);
		cyclicButton.setText("Beginning of cycle");
		cyclicButton.addActionListener(CycleResetListener.getInstance(cyclicButtons));
		
		cyclicButtons.add(cyclicButton);
		buttonPanel.add(cyclicButton);
		
		JButton removeButton = new JButton("Remove Action");
		removeButton.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				cyclicButtons.remove(cyclicButton);
				getThis().setVisible(false);
				menu.remove(getThis());
				actions.remove(data);
				menu.setVisible(false);
				menu.setVisible(true);
			}
			
		});
		buttonPanel.add(removeButton);
		
		add(infoPanel);
		add(buttonPanel);
		
		setBorder(BorderFactory.createLineBorder(Color.BLACK));
		setVisible(true);
	}
	
	ActionMetadataPanel getThis() {
		return this;
	}
}
