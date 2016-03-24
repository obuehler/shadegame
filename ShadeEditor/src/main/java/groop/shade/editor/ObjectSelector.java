package groop.shade.editor;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.AbstractButton;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JMenuItem;
import javax.swing.border.EmptyBorder;

public abstract class ObjectSelector {

	EditorItems value;
	AbstractButton button;

	public ObjectSelector() {
	}

	public ObjectSelector(StageType e) {
		value = e.getValue();
	}

	public void setSelectAction(final StageEditor s) {
		button.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.selectedOption = value;
				s.selectLabel.setText(value.getName());
			}
		});
	}

	static final class CustomMenuItem extends ObjectSelector {

		public CustomMenuItem(StageType e, StageEditor s) {
			super(e);
			button = new JMenuItem(e.getName());
			setSelectAction(s);
		}

	}

	static final class BirdButton extends ObjectSelector {

		public BirdButton(StageEditor s) {
			value = EditorItems.BIRDSITE;
			button = new JButton("Bird");
			setSelectAction(s);
		}

	}

	static final class EraserButton extends ObjectSelector {

		public EraserButton(StageEditor s) {
			value = EditorItems.ERASER;
			// image from @link
			// http://onlinehelp.smarttech.com/english/mobile/nbmac/11_0_0/Advanced/Content/Resources/Images/ButtonDelete.png
			button = new JButton(new ImageIcon(Constants.ASSETS_PATH() + "images/editor/delete.png"));

			button.setBorder(new EmptyBorder(4, 3, 4, 3));

			setSelectAction(s);
		}

	}

	static final class SelectorButton extends ObjectSelector {

		public SelectorButton(StageEditor s) {
			value = EditorItems.SELECTOR;

			// mouse cursor from @link
			// http://telcontar.net/Misc/screeniecursors/Cursor%20arrow%20Aero.png
			button = new JButton(new ImageIcon(Constants.ASSETS_PATH() + "images/editor/mouse_cursor.png"));

			button.setBorder(new EmptyBorder(1, 3, 0, 3));

			setSelectAction(s);
		}
	}
}
