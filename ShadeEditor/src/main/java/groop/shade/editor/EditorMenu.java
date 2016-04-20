package groop.shade.editor;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.SwingWorker;
import groop.shade.editor.ObjectSelector.CustomMenuItem;
import groop.shade.editor.ObjectSelector.StaticObjectButton;

@SuppressWarnings("serial")
public class EditorMenu extends JMenu {
	
	public static class BuildingImageCopier extends SwingWorker<Boolean, Boolean> {

		boolean returnValue;
		String originalPath;
		String originalShadowPath;
		StaticObjectType type;
		StageEditor editor;
		EditorMenu menu;
		
		BuildingImageCopier(String path, String shadowPath, StaticObjectType t, StageEditor e, EditorMenu m) {
			originalPath = path;
			originalShadowPath = shadowPath;
			type = t;
			editor = e;
			menu = m;
		}

		@Override
		protected Boolean doInBackground() {
			try {
				if (type.imageFormat == null) {
					type.imageFormat = originalPath.substring(originalPath.lastIndexOf(".") + 1);
				}
				if (type.shadowImageFormat == null) {
					type.shadowImageFormat = originalShadowPath.substring(originalPath.lastIndexOf(".") + 1);
				}
				Files.copy(Paths.get(originalPath), Paths.get(type.getIconPath()),
						StandardCopyOption.REPLACE_EXISTING);
				Files.copy(Paths.get(originalShadowPath), Paths.get(type.getIconShadowPath()),
						StandardCopyOption.REPLACE_EXISTING);
				returnValue = true;
			} catch (IOException e) {
				e.printStackTrace();
				returnValue = false;
			}
			return returnValue;
		}

		@Override
		protected void done() {
			if (returnValue) {
				editor.staticObjectTypes.types.add(type);
				addStaticObjectTypeButton(editor, menu, type);
			} else {
				Helpers.couldNotCopyObjectImage(editor);
			}
		}
		
	}
	
	public static EditorMenu obstacleMenu(StageEditor s) {
		EditorMenu menu = new EditorMenu("Obstacle");
		for (ObstacleType obstacleType : ObstacleType.values()) {
			menu.add(new CustomMenuItem(obstacleType, s));
		}
		return menu;
	}
	
	public static EditorMenu staticObjectsMenu(final StageEditor s) {
		final EditorMenu menu = new EditorMenu("Static Objects");
		menu.add(new JMenuItem("Add Static Object...")).addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				final String name = MetadataSetters.inputStaticObjectName(s.staticObjectTypes.types);
				if (name != null) {
					JOptionPane.showMessageDialog(s, "Select the image for the object.");
					JFileChooser chooser = new JFileChooser() {

						@Override
						public void approveSelection() {
							super.approveSelection();
							final String objectPath = getSelectedFile().getAbsolutePath();
							if (objectPath != null) {
								JOptionPane.showMessageDialog(s, "Select the image for the shadow.");
								JFileChooser chooser2 = new JFileChooser() {

									@Override
									public void approveSelection() {
										super.approveSelection();
										String shadowPath = getSelectedFile().getAbsolutePath();
										if (shadowPath != null) {
											StaticObjectType newType = new StaticObjectType(name, 1.0);
											BuildingImageCopier copier = new BuildingImageCopier(objectPath, shadowPath, newType, s, menu);
											copier.execute();
										}
									}
								};
								Helpers.openImageSelectionScreen(chooser2);
							}
						}
					};
					Helpers.openImageSelectionScreen(chooser);
				}
			}
			
		});
		menu.add(new JSeparator(JSeparator.HORIZONTAL));
		for (final StaticObjectType t : s.staticObjectTypes.types) {
			addStaticObjectTypeButton(s, menu, t);
		}
		return menu;
	}

	protected static void addStaticObjectTypeButton(final StageEditor s,
			final EditorMenu menu, final StaticObjectType t) {
		final JPanel buttonPanel = new JPanel();
		buttonPanel.setMinimumSize(new Dimension(80, 50));
		buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.LINE_AXIS));
		buttonPanel.add((JButton)((new StaticObjectButton(t, s)).button));
		JButton removeButton = new JButton("Remove");
		removeButton.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				int answer = JOptionPane.showConfirmDialog(s, 
						"Are you sure you want to delete " + t.getName() + "?",
						"Attention!", JOptionPane.YES_NO_OPTION);
				if (answer == JOptionPane.YES_OPTION) {
					s.staticObjectTypes.types.remove(t);
					buttonPanel.setVisible(false);
					menu.remove(buttonPanel);
					menu.setVisible(false);
					menu.setVisible(true);
				}
			}
			
		});
		buttonPanel.add(removeButton);
		buttonPanel.setVisible(true);
		menu.add(buttonPanel);
		menu.setVisible(false);
		menu.setVisible(true);
	}

	public static EditorMenu editMenu(final StageEditor s) {
		EditorMenu menu = new EditorMenu("Edit");

		JMenuItem menuItem = new JMenuItem("Undo");
		menuItem.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.undo();
			}

		});
		menu.add(menuItem);

		menuItem = new JMenuItem("Redo");
		menuItem.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.redo();
			}

		});
		menu.add(menuItem);

		menuItem = new JMenuItem("Name...");
		menuItem.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.editStageName();
			}

		});
		menu.add(menuItem);

		menuItem = new JMenuItem("Size...");
		menuItem.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.editStageSize();
			}

		});
		menu.add(menuItem);

		return menu;
	}

	public static EditorMenu fileMenu(final StageEditor s) {
		// Save map(you can save into any directory, the coders will insert it
		// manually in the game), export image, open, new, exit
		EditorMenu menu = new EditorMenu("File");

		JMenuItem menuItem = new JMenuItem("New");
		menuItem.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				new StageEditor();
			}

		});
		menu.add(menuItem);

		menuItem = new JMenuItem("Open");
		menuItem.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.open(true);
			}

		});

		menu.add(menuItem);

		menuItem = new JMenuItem("Save into stages");
		menuItem.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.save();
			}

		});
		menu.add(menuItem);

		menuItem = new JMenuItem("Import " + Constants.DOTTED_STAGE_FILE_EXTENSION() + " file...");
		menuItem.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.importStompFile();
			}

		});
		menu.add(menuItem);

		menuItem = new JMenuItem("Export " + Constants.DOTTED_STAGE_FILE_EXTENSION() + " file...");
		menuItem.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.saveAs();
			}

		});
		menu.add(menuItem);

		menuItem = new JMenuItem("Export image...");
		menuItem.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				s.exportImage();
			}

		});
		menu.add(menuItem);

		return menu;
	}

	public EditorMenu(String text) {
		super(text);
	}

	public JMenuItem add(CustomMenuItem c) {
		return add((JMenuItem) c.button);
	}
}
