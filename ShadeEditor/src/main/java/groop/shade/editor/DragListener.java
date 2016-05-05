/**
 * Code borrowed from
 * {@link http://www.javaexamples.org/java/java.awt.dnd/how-to-implement-draggesturelistener-interface.html}
 * and edited as necessary to our needs.
 */

package groop.shade.editor;

import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.dnd.DragGestureEvent;
import java.awt.dnd.DragGestureListener;
import java.awt.dnd.DragSourceContext;
import java.awt.dnd.DragSourceDragEvent;
import java.awt.dnd.DragSourceDropEvent;
import java.awt.dnd.DragSourceEvent;
import java.awt.dnd.DragSourceListener;
import java.awt.dnd.DropTarget;
import java.awt.dnd.DropTargetDragEvent;
import java.awt.dnd.DropTargetDropEvent;
import java.awt.dnd.DropTargetEvent;
import java.awt.dnd.DropTargetListener;
import java.awt.dnd.InvalidDnDOperationException;

import groop.shade.editor.StageEditor.BackgroundPanel;

public class DragListener implements DragGestureListener, DragSourceListener, DropTargetListener, Transferable {

	static final DataFlavor[] dataflavor = { null };
	Object object;
	static {
		try {
			dataflavor[0] = new DataFlavor(DataFlavor.javaJVMLocalObjectMimeType);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public Object getTransferData(DataFlavor flavor) {
		if (flavor.isMimeTypeEqual(DataFlavor.javaJVMLocalObjectMimeType)) {
			return object;
		} else {
			return null;
		}
	}

	public DataFlavor[] getTransferDataFlavors() {
		return dataflavor;
	}

	public boolean isDataFlavorSupported(DataFlavor flavor) {
		return flavor.isMimeTypeEqual(DataFlavor.javaJVMLocalObjectMimeType);
	}

	@Override
	public void drop(DropTargetDropEvent dtde) {
		try {
			Object source = dtde.getTransferable().getTransferData(dataflavor[0]);
			Object target = dtde.getSource();
			StageObjectIcon component = (StageObjectIcon) ((DragSourceContext) source).getComponent();
			BackgroundPanel oldContainer = (BackgroundPanel) component.getParent();
			BackgroundPanel newContainer = (BackgroundPanel) ((DropTarget) target).getComponent();
			if (oldContainer == newContainer) {
				int x = dtde.getLocation().x;
				int y = dtde.getLocation().y;
				boolean success = inBounds(component, x, y);
				if (success) {
					dtde.acceptDrop(dtde.getDropAction());
					component.moveIcon(x, y);
				} else {
					dtde.rejectDrop();
				}
				dtde.dropComplete(success);

			} else {
				newContainer.transfer(component);
				oldContainer.validate();
				oldContainer.repaint();
				newContainer.validate();
				newContainer.repaint();
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	private boolean inBounds(StageObjectIcon icon, int x, int y) {
		return x <= icon.getParent().getWidth() + icon.getIcon().getIconWidth() / 2
				&& x >= -(icon.getIcon().getIconWidth() / 2)
				&& y <= icon.getParent().getHeight() + icon.getIcon().getIconHeight() / 2
				&& y >= -(icon.getIcon().getIconHeight() / 2);

	}

	// DragGestureListener method.
	public void dragGestureRecognized(DragGestureEvent dge) {
		try {
			dge.startDrag(null, this, this);
		} catch (InvalidDnDOperationException e) {
		}
	}

	// DragSourceListener methods.
	public void dragDropEnd(DragSourceDropEvent dsde) {
	}

	public void dragEnter(DragSourceDragEvent dsde) {
	}

	public void dragExit(DragSourceEvent dse) {
	}

	public void dragOver(DragSourceDragEvent dsde) {
		object = dsde.getSource();
	}

	public void dropActionChanged(DragSourceDragEvent dsde) {
	}

	// DropTargetListener methods.
	public void dragEnter(DropTargetDragEvent dtde) {
	}

	public void dragExit(DropTargetEvent dte) {
	}

	public void dragOver(DropTargetDragEvent dtde) {
		dtde.acceptDrag(dtde.getDropAction());
	}

	public void dropActionChanged(DropTargetDragEvent dtde) {
		dtde.acceptDrag(dtde.getDropAction());
	}

}
