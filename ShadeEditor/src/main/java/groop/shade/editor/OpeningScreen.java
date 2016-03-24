package groop.shade.editor;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;

import javax.swing.AbstractAction;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JRootPane;
import javax.swing.WindowConstants;

public class OpeningScreen extends JFrame {

	public OpeningScreen() {
		setUndecorated(true);
		getRootPane().setWindowDecorationStyle(JRootPane.PLAIN_DIALOG);
		setAlwaysOnTop(true);
		setTitle(StageEditor.TITLE);
		setSize(210, 55);
		setResizable(false);
		setLocationRelativeTo(null);
		setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
		getContentPane().setLayout(Constants.BORDER_LAYOUT);

		JButton newStage = new JButton("New Stage");
		newStage.addActionListener(new AbstractAction() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				new StageEditor(getThis(), true);
			}

		});

		JButton loadStage = new JButton("Load Stage");
		loadStage.addActionListener(new AbstractAction() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				new StageEditor(getThis(), false);
			}

		});

		getContentPane().add(newStage, BorderLayout.WEST);
		getContentPane().add(loadStage, BorderLayout.CENTER);

		setVisible(true);
	}

	public OpeningScreen getThis() {
		return this;
	}

	public static void main(String[] args) {
		new OpeningScreen();
	}

}
