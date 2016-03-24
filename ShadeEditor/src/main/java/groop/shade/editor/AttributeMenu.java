package groop.shade.editor;

import java.awt.Component;
import java.awt.Dimension;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JMenuBar;
import javax.swing.JPanel;

@SuppressWarnings("serial")
public class AttributeMenu extends JPanel {

	JMenuBar menuBar;

	public AttributeMenu() {
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		menuBar = new JMenuBar();
		menuBar.setLayout(new BoxLayout(menuBar, BoxLayout.Y_AXIS));
		super.add(menuBar);
		JPanel panel2 = new JPanel();
		super.add(panel2);
		super.add(Box.createVerticalGlue());
		setPreferredSize(new Dimension(200, 300));
		setMinimumSize(new Dimension(200, 300));
		setSize(getPreferredSize());
	}

	@Override
	public Component add(Component c) {
		return menuBar.add(c);
	}

}
