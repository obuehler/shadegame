package groop.shade.editor;

import java.awt.event.ActionEvent;

import javax.swing.AbstractAction;

@SuppressWarnings("serial")
public class EditorActions {

	public static final AbstractAction undoAction(final StageEditor s) {
		return new AbstractAction() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.undo();
			}
		};
	}

	public static final AbstractAction redoAction(final StageEditor s) {
		return new AbstractAction() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.redo();
			}
		};
	}

	public static final AbstractAction fullScreenAction(final StageEditor s) {
		return new AbstractAction() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.changeFullscreen();
			}
		};
	}

	public static final AbstractAction newAction() {
		return new AbstractAction() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				new StageEditor();
			}
		};
	}

	public static final AbstractAction openAction(final StageEditor s) {
		return new AbstractAction() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.open(true);
			}
		};
	}

	public static final AbstractAction saveAction(final StageEditor s) {
		return new AbstractAction() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.save();
			}
		};
	}

	public static final AbstractAction exportAction(final StageEditor s) {
		return new AbstractAction() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.saveAs();
			}
		};
	}

}
