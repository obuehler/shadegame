package groop.shade.editor;

import java.awt.Component;
import java.awt.Dimension;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JMenuBar;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ScrollPaneLayout;

@SuppressWarnings("serial")
public class AttributeMenu extends JPanel {

	JMenuBar menuBar;

	public AttributeMenu() {
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		JPanel viewportPanel = new JPanel();
		viewportPanel.setLayout(Constants.FLOW_LAYOUT);
		JPanel subPanel = new JPanel();
		subPanel.setLayout(new BoxLayout(subPanel, BoxLayout.Y_AXIS));
		viewportPanel.add(subPanel);
		JScrollPane scrollPane = new JScrollPane();
		super.add(scrollPane);
		scrollPane.setLayout(new ScrollPaneLayout());
		scrollPane.getViewport().setView(viewportPanel);
		scrollPane.setVisible(true);
		menuBar = new JMenuBar();
		menuBar.setLayout(new BoxLayout(menuBar, BoxLayout.Y_AXIS));
		subPanel.add(menuBar);
		JPanel panel2 = new JPanel();
		subPanel.add(panel2);
		subPanel.add(Box.createVerticalGlue());
		setPreferredSize(new Dimension(200, 300));
		setMinimumSize(new Dimension(200, 300));
		setSize(getPreferredSize());
	}

	@Override
	public Component add(Component c) {
		return menuBar.add(c);
	}

}
