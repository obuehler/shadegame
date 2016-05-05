package groop.shade.editor;

import java.awt.Dimension;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JLabel;

@SuppressWarnings("serial")
public class NewStageIcon extends JLabel {
	StageEditor editor;
	protected int xCoordinate;
	protected int yCoordinate;
	
	public NewStageIcon(int x, int y, StageEditor s) {
		setBasicAttributes(x, y, s);
	}
	
	public NewStageIcon(StageEditor s, String imagePath) {
		setBasicAttributes(s.getWidth() / 2, s.getHeight() / 2, s);
		setDisplayIcon(imagePath);
	}
	
	public NewStageIcon(int x, int y, StageEditor s, String imagePath) {
		setBasicAttributes(x, y, s);
		setDisplayIcon(imagePath);
	}

	public void setBasicAttributes(int x, int y, StageEditor s) {
		setCoordinates(x, y);
		editor = s;
	}
	
	public void setCoordinates(int x, int y) {
		xCoordinate = x;
		yCoordinate = y;
	}
	

	@Override
	public void setLocation(int x, int y) {
		setCoordinates(x, y);
		setLocation();
	}

	public void setLocation() {
		super.setLocation(xCoordinate - getIcon().getIconWidth() / 2, yCoordinate - getIcon().getIconHeight() / 2);
	}
	

	protected void setDisplayIcon(BufferedImage original, float scale) {
		Image scaled = original.getScaledInstance((int) (original.getWidth() * scale),
				(int) (original.getHeight() * scale), Image.SCALE_SMOOTH);
		setDisplayIcon(scaled);
	}

	protected void setDisplayIcon(Image image) {
		ImageIcon icon = new ImageIcon(image);
		setIcon(icon);
		Dimension size = new Dimension(icon.getIconWidth(), icon.getIconHeight());
		setSize(size);
		setPreferredSize(size);
	}
	
	protected void setDisplayIcon(String path) {
		setDisplayIcon(path, 1.0f);
	}

	protected void setDisplayIcon(String path, float scale) {
		try {
			BufferedImage original = ImageIO.read(new File(path));
			setDisplayIcon(original, scale);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
