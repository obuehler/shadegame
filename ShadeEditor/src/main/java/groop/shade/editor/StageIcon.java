package groop.shade.editor;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.border.LineBorder;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public abstract class StageIcon extends JLabel {

	protected static final LineBorder SELECTED_BORDER = new LineBorder(Color.ORANGE, 5);

	Vector2 object;
	StageEditor editor;
	protected int xCoordinate;
	protected int yCoordinate;
	AttributeMenu menu;

	// CONSTRUCTORS

	/** Constructor to call for icons that are not ChangeableIcons */
	public StageIcon(int x, int y, StageEditor s) {
		preInitializeIcon(x, y, s);
		setObject(x, y);
		initializeIcon();
	}

	/** Constructor to call for ChangeableIcons */
	public StageIcon(StageType e, int x, int y, StageEditor s) {
		preInitializeIcon(x, y, s);
		setObject(e, x, y);
		initializeIcon();
	}

	/** Constructor to call when loading a preexisting .stomp file */
	public StageIcon(Vector2 o, StageEditor e) {
		preInitializeIcon(Math.round(o.x * Constants.DEFAULT_SCALE),
				e.backgroundPanel.getHeight() - Math.round(o.y * Constants.DEFAULT_SCALE), e);
		object = o;
		initializeIcon();
	}

	// METHODS

	protected void preInitializeIcon(int x, int y, StageEditor s) {
		setCoordinates(x, y);
		editor = s;
		menu = new AttributeMenu();
		editor.menuPanel.add(menu, BorderLayout.CENTER);
	}

	protected void initializeIcon() {
		setDisplayIcon();
		setLocation();
		refreshToolTipText();
		setMenu();
		setVisible(true);
	}

	public void setCoordinates(int x, int y) {
		xCoordinate = x;
		yCoordinate = y;
	}

	protected void setDisplayIcon(String path) {
		setDisplayIcon(path, 1.0);
	}

	protected void setDisplayIcon(String path, double scale) {
		try {
			BufferedImage original = ImageIO.read(new File(path));
			Image scaled = original.getScaledInstance((int) (original.getWidth() * scale),
					(int) (original.getHeight() * scale), Image.SCALE_SMOOTH);
			ImageIcon icon = new ImageIcon(scaled);
			setIcon(icon);
			Dimension size = new Dimension(icon.getIconWidth(), icon.getIconHeight());
			setSize(size);
			setPreferredSize(size);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void setLocation(int x, int y) {
		setCoordinates(x, y);
		setLocation();
	}

	public void setLocation() {
		super.setLocation(xCoordinate - getIcon().getIconWidth() / 2, yCoordinate - getIcon().getIconHeight() / 2);
	}

	/**
	 * Moves the icon to the (x,y) on the panel and resets the associated
	 * object's position.
	 */
	public void moveIcon(int x, int y) {
		setLocation(x, y);
		editor.backgroundPanel.repaint();
		object.set(stagePosition());

	}

	protected Vector2 stagePosition() {
		return new Vector2(xCoordinate, editor.backgroundPanel.getHeight() - yCoordinate)
				.scl(1f / Constants.DEFAULT_SCALE);
	}

	@Override
	public Point getLocation() {
		return new Point(xCoordinate, yCoordinate);
	}

	/**
	 * Displays the customize menu at the top and produces an outline indicating
	 * the icon has been selected.
	 */
	public void select() {
		editor.selectedIcon = this;
		setBorder(SELECTED_BORDER);
		menu.setVisible(true);
	}

	/** Sets the object with given type and adds it to the stage. */
	protected abstract void setObject(StageType e, int x, int y);

	/** Sets the object and adds it to the stage. */
	protected abstract void setObject(int x, int y);

	/** Calls setDisplayIcon(String). */
	protected abstract void setDisplayIcon();

	/** Refreshes the tooltip text. */
	protected abstract void refreshToolTipText();

	/**
	 * Deletes the icon from the background panel and its associated object from
	 * the stage.
	 */
	public abstract void delete();

	/** Sets the attribute menu for this icon. */
	protected abstract void setMenu();

	/**
	 * Changes the assigned editor after a drag and drop operation. This also
	 * includes deleting the associated object from the stage in the old editor
	 * and creating a new one in the stage in the new editor.
	 */
	public abstract void changeEditor(StageEditor e);

}
