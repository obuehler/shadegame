package groop.shade.editor;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.Toolkit;
import java.awt.dnd.DnDConstants;
import java.awt.dnd.DragSource;
import java.awt.dnd.DropTarget;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;

import javax.imageio.ImageIO;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.KeyStroke;
import javax.swing.ScrollPaneLayout;
import javax.swing.SwingConstants;
import javax.swing.SwingWorker;
import javax.swing.WindowConstants;
import javax.swing.border.EmptyBorder;
import javax.swing.border.EtchedBorder;
import javax.swing.filechooser.FileNameExtensionFilter;

import com.badlogic.gdx.utils.Json;
import com.badlogic.gdx.utils.JsonWriter;

import groop.shade.editor.ObjectSelector.CasterButton;
import groop.shade.editor.ObjectSelector.EraserButton;
import groop.shade.editor.ObjectSelector.PlayerButton;
import groop.shade.editor.ObjectSelector.SelectorButton;

@SuppressWarnings("serial")
public class StageEditor extends JFrame {

	public static class StaticObjectTypes {
	/** The list of static object types, loaded from/onto the static object
	 * constants JSON file */
		ArrayList<StaticObjectType> types;
		
		StaticObjectTypes() {}
		
		StaticObjectTypes(ArrayList<StaticObjectType> list) { types = list; }
	}
	
	static final String TITLE = "Shade Level Editor";

	private static final Dimension MINIMUM_WINDOW_SIZE = new Dimension(700, 400);

	public static int instancesRunning = 0;

	class BackgroundPanel extends JPanel {

		DragListener editorListener;
		DragSource dragSource;
		DropTarget dropTarget;

		public BackgroundPanel() {
			setLayout(null);
			setSize(stage.pixelSize);
			setPreferredSize(stage.pixelSize);
			bufferedImage = new BufferedImage(getWidth(), getHeight(), BufferedImage.TYPE_INT_ARGB);
			setBorder(BorderFactory.createEtchedBorder(EtchedBorder.RAISED));
			editorListener = new DragListener();
			dragSource = new DragSource();
			dropTarget = new DropTarget(this, DnDConstants.ACTION_MOVE, editorListener);
			addMouseListener(mouseListener);
			setVisible(true);
		}

		@Override
		public void paintComponent(Graphics g) {
			super.paintComponent(g);

			if (bufferedImage != null) {
				Graphics2D g2 = (Graphics2D) g;
				g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
				g2.drawImage(bufferedImage, 0, 0, getWidth(), getHeight(), this);
				stage.scalex = stage.pixelSize.width / (float) bufferedImage.getWidth();
				stage.scaley = stage.pixelSize.height / (float) bufferedImage.getHeight();
			}
		}

		@Override
		public Component add(Component component) {
			component.addMouseListener(mouseListener);
			dragSource.createDefaultDragGestureRecognizer(component, DnDConstants.ACTION_MOVE, editorListener);
			super.add(component);
			repaint();
			return component;
		}

		public void add(StageIcon icon) {
			add((JLabel) icon);
			getThis().menuPanel.add(icon.menu).setVisible(false);
		}

		/** For drag & drop between frames */
		public void transfer(StageIcon icon) {
			try {
				icon.removeMouseListener(icon.getMouseListeners()[0]);
			} catch (Exception e) {
				e.printStackTrace();
			}
			add(icon);
			icon.changeEditor(getThis());
		}

		@Override
		public void remove(Component component) {
			super.remove(component);
			repaint();
		}

	}

	public final MouseListener mouseListener = new MouseListener() {
		@Override
		public void mouseClicked(MouseEvent arg0) {
		}

		@Override
		public void mouseEntered(MouseEvent arg0) {
		}

		@Override
		public void mouseExited(MouseEvent arg0) {
		}

		@Override
		public void mousePressed(MouseEvent arg0) {
		}

		@Override
		public void mouseReleased(MouseEvent arg0) {
			int x = arg0.getX();
			int y = arg0.getY();
			try {
				try {
					selectedOption.clickAction(x, y, getThis(), (StageIcon) arg0.getComponent());
				} catch (ClassCastException e) {
					e.printStackTrace();
					selectedOption.clickAction(x, y, getThis(), null);
				}
			} catch (NullPointerException e) {
				e.printStackTrace();
			}
		}
	};
	
	StaticObjectTypes staticObjectTypes;

	/** The currently loaded stage */
	GameStage stage;

	/** The icon representing the current player site */
	PlayerSiteIcon playerSiteIcon;

	/** The icon representing the current caster site */
	CasterSiteIcon casterSiteIcon;

	/** The background panel */
	BackgroundPanel backgroundPanel;

	/** Panel containing the side menu */
	JPanel menuPanel;
	
	/** Side menu */
	AttributeMenu sideMenu;

	/** The stage's content pane */
	JScrollPane scrollPane;

	/** The panel scrollPane's viewport is set on. */
	JPanel viewportPanel;

	/** Text indicating which element is currently selected */
	JLabel selectLabel;

	/** The currently selected option in the editor. */
	EditorItem selectedOption;

	/** Path that the background image originally came from. */
	String backgroundPath;

	private BufferedImage bufferedImage;

	public StageIcon selectedIcon;

	/**
	 * Called when a new editor is created by choosing "New" from the "File"
	 * menu or by pressing Ctrl+N / Cmd+N
	 */
	public StageEditor() {
		String name = MetadataSetters.inputName();
		if (name != null) {
			Dimension size = MetadataSetters.inputSize();
			if (size != null) {
				stage = new GameStage(name, size);
				loadStaticObjectTypes();
				setAttributes();
			}
		}
	}

	

	/**
	 * Created from the OpeningScreen.
	 * 
	 * @param newStage
	 *            = true if creating newStage, false if opening one
	 */
	public StageEditor(OpeningScreen openingScreen, boolean newStage) {
		openingScreen.setVisible(false);
		if (newStage) {
			String name = MetadataSetters.inputName();
			if (name != null) {
				Dimension size = MetadataSetters.inputSize();
				if (size != null) {
					stage = new GameStage(name, size);
					loadStaticObjectTypes();
					setAttributes();
					openingScreen.dispose();
				} else {
					openingScreen.setVisible(true);
				}
			} else {
				openingScreen.setVisible(true);
			}
		} else {
			open(false);
			openingScreen.dispose();
		}
	}

	/** Called when loading an existing stage to a new window */
	public StageEditor(GameStage inputStage) {
		stage = inputStage;
		loadStaticObjectTypes();
		newEditorWithStage();
		setVisible(true);
	}

	/** Called when loading an existing stage to the existing window */
	private void newEditorWithStage() {
		backgroundPath = stage.fullBackgroundPath();
		setAttributes();
		setNewBackgroundImage();
		if (stage.playerSite != null) {
			playerSiteIcon = new PlayerSiteIcon(stage.playerSite, this);
			backgroundPanel.add(playerSiteIcon);
		}
		if (stage.casterSite != null) {
			casterSiteIcon = new CasterSiteIcon(stage.casterSite, this);
			backgroundPanel.add(casterSiteIcon);
		}
		for (TreeSite t : stage.cars) {
			backgroundPanel.add(new TreeIcon(t, this));
		}
		for (TreeSite t : stage.pedestrians) {
			backgroundPanel.add(new TreeIcon(t, this));
		}
		for (StaticObjectSite s : stage.staticObjects) {
			backgroundPanel.add(new StaticObjectIcon(s, this));
		}
		repaint();
	}

	public StageEditor getThis() {
		return this;
	}

	void editStageName() {
		String name = MetadataSetters.inputName();
		if (name != null) {
			stage.name = name;
			setTitle(TITLE + " - " + name);
		}
	}

	private void setAttributes() {
		instancesRunning++;
		getContentPane().setLayout(new BoxLayout(getContentPane(), BoxLayout.X_AXIS));
		setLocationRelativeTo(null);
		setResizable(true);
		setExtendedState(JFrame.MAXIMIZED_BOTH);
		menuPanel = new JPanel();
		menuPanel.setLayout(new BoxLayout(menuPanel, BoxLayout.Y_AXIS));
		menuPanel.setPreferredSize(new Dimension(200, 2000)); // 2000 is an
																// arbitrary
																// large number
																// that will
																// definitely
																// fill the
																// screen
																// vertically
		menuPanel.setMinimumSize(new Dimension(200, 1));
		menuPanel.setSize(menuPanel.getPreferredSize());
		menuPanel.setBackground(Color.DARK_GRAY);
		
		sideMenu = new AttributeMenu();
		
		menuPanel.add(sideMenu);
		
		menuPanel.setVisible(true);
		getContentPane().add(menuPanel);
		backgroundPanel = new BackgroundPanel();
		viewportPanel = new JPanel();
		viewportPanel.setLayout(new FlowLayout());
		viewportPanel.add(backgroundPanel);
		scrollPane = new JScrollPane();
		getContentPane().add(scrollPane);
		scrollPane.setLayout(new ScrollPaneLayout());
		scrollPane.getViewport().setView(viewportPanel);
		scrollPane.setVisible(true);
		playerSiteIcon = null;
		casterSiteIcon = null;
		EditorMenuBar menuBar = new EditorMenuBar();
		selectedIcon = null;
		selectedOption = EditorItems.SELECTOR;
		selectLabel = new JLabel(EditorItems.SELECTOR.getName(), SwingConstants.LEFT);
		selectLabel.setPreferredSize(new Dimension(150, 24));
		selectLabel.setMaximumSize(new Dimension(150, 24));
		selectLabel.setBorder(new EmptyBorder(0, 20, 0, 20));
		menuBar.add(EditorMenu.fileMenu(this));
		menuBar.add(EditorMenu.editMenu(this));
		menuBar.add(EditorMenu.obstacleMenu(this));
		menuBar.add(EditorMenu.staticObjectsMenu(this));
		menuBar.add(new PlayerButton(this));
		menuBar.add(new CasterButton(this));
		menuBar.add(backgroundButton());
		menuBar.add(Box.createHorizontalGlue());
		menuBar.add(new SelectorButton(this));
		menuBar.add(new EraserButton(this));
		menuBar.add(selectLabel);
		menuBar.add(testButton());
		setJMenuBar(menuBar);
		setKeyboardShortcuts();
		setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
		addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent e) {
				int confirm = Helpers.confirmQuit(getThis());
				switch (confirm) {
				case JOptionPane.YES_OPTION:
					save();
					quitEditor();
					break;
				case JOptionPane.NO_OPTION:
					quitEditor();
					break;
				default:
					break;
				}
			}

		});
		setMinimumSize(MINIMUM_WINDOW_SIZE);
		setVisible(true);
	}

	private void quitEditor() {
		dispose();
		instancesRunning--;
		if (instancesRunning <= 0) {
			String output = new Json(JsonWriter.OutputType.json).toJson(staticObjectTypes);
			if (output.length() == 0) {
				Helpers.couldNotSaveFile(this);
			} else {
				try {
					JsonWriter jason = new JsonWriter(new FileWriter(new File(Paths.get(Constants.STATIC_OBJECT_CONSTANTS_FILE_PATH()).toString())));
					jason.write(output);
					jason.close();
				} catch (IOException e) {
					e.printStackTrace();
					Helpers.couldNotSaveStaticObjectTypes(this);
				}
			}
			System.exit(0);
		}
	}

	private JButton testButton() {
		JButton button = new JButton("Test Map");
		button.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				int save = JOptionPane.showConfirmDialog(getThis(),
						"The stage needs to be saved before testing. Go ahead?");
				if (save == JOptionPane.YES_OPTION) {
					save();
					// new LwjglApplication(new GDXRoot(stage),
					// Constants.STANDARD_CONFIG); THIS NORMALLY STARTS THE GAME
				}
			}
		});
		return button;
	}

	private JButton backgroundButton() {
		JButton button = new JButton("Background...");
		button.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				JFileChooser chooser = new JFileChooser() {

					@Override
					public void approveSelection() {
						super.approveSelection();
						backgroundPath = getSelectedFile().getAbsolutePath();
						setNewBackgroundImage();
					}
				};
				Helpers.openImageSelectionScreen(chooser);
			}
		});
		return button;
	}

	void editStageSize() {
		Dimension size = MetadataSetters.inputSize();
		if (size != null && stage.pixelSize != size) {
			stage.pixelSize = size;
			// We don't remove objects that lie outside of the new size, just so
			// we can reuse them if we want to.
			// Remove and re-add backgroundPanel to center it in viewportPanel
			backgroundPanel.setVisible(false);
			backgroundPanel.setSize(size);
			backgroundPanel.setPreferredSize(size);

			backgroundPanel.setVisible(true);
		}
	}

	/**
	 * A cool feature to have. Code to produce image obtained from
	 * {@link http://stackoverflow.com/questions/19621105/save-image-from-jpanel-after-draw}
	 */
	void exportImage() {

		final BufferedImage image = new BufferedImage(backgroundPanel.getWidth(), backgroundPanel.getHeight(),
				BufferedImage.TYPE_INT_ARGB);
		backgroundPanel.paint(image.getGraphics());
		JFileChooser chooser = Helpers.chooseFile(this);
		chooser.setAcceptAllFileFilterUsed(false);
		chooser.addChoosableFileFilter(new FileNameExtensionFilter(
				"Portable Network Graphics (*." + Constants.EXPORT_IMAGE_FORMAT + ")", Constants.EXPORT_IMAGE_FORMAT));

		int returnVal = chooser.showSaveDialog(this);
		if (returnVal == JFileChooser.APPROVE_OPTION) {
			final String savePath = chooser.getSelectedFile().getAbsolutePath() + "." + Constants.EXPORT_IMAGE_FORMAT;
			SwingWorker<Boolean, Boolean> worker = new SwingWorker<Boolean, Boolean>() {

				boolean returnValue;

				@Override
				protected Boolean doInBackground() {
					try {
						ImageIO.write(image, Constants.EXPORT_IMAGE_FORMAT, new File(savePath));
						returnValue = true;
					} catch (IOException e) {
						e.printStackTrace();
						returnValue = false;
					}
					return returnValue;
				}

				@Override
				protected void done() {
					if (!returnValue) {
						Helpers.couldNotExportImage(getThis());
					}
				}

			};

			worker.execute();
		}
	}

	public void loadIntoExistingInstance(File selectedFile) {
		Json json2 = new Json();
		try {
			stage = json2.fromJson(GameStage.class, new FileReader(selectedFile));
			newEditorWithStage();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			Helpers.couldNotOpenFile(getThis());
		}
	}

	void open(final boolean isVisible) {
		JFileChooser chooser = new JFileChooser(Constants.STAGES_PATH()) {
			@Override
			public void approveSelection() {
				if (isVisible) {
					int confirm = Helpers.confirmNewEditor(getThis());
					switch (confirm) {
					case JOptionPane.YES_OPTION:
						super.approveSelection();
						Json json = new Json();
						try {
							new StageEditor(json.fromJson(GameStage.class, new FileReader(getSelectedFile())));
						} catch (FileNotFoundException e) {
							e.printStackTrace();
							Helpers.couldNotOpenFile(getThis());
						}
						break;
					case JOptionPane.NO_OPTION:
						super.approveSelection();
						loadIntoExistingInstance(getSelectedFile());
						break;
					case JOptionPane.CANCEL_OPTION:
						cancelSelection();
						break;
					default:
						break;
					}
				} else {
					super.approveSelection();
					loadStaticObjectTypes();
					loadIntoExistingInstance(getSelectedFile());
				}
			}
		};
		chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
		chooser.setAcceptAllFileFilterUsed(false);
		chooser.addChoosableFileFilter(new FileNameExtensionFilter(
				"Troll Stomp Stage Files (*" + Constants.DOTTED_STAGE_FILE_EXTENSION() + ")",
				Constants.STAGE_FILE_EXTENSION));
		chooser.showOpenDialog(this);
	}

	public void importStompFile() {
		JFileChooser chooser = new JFileChooser(Constants.STAGES_PATH()) {
			@Override
			public void approveSelection() {
				super.approveSelection();
				loadIntoExistingInstance(getSelectedFile());
			}
		};
		chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
		chooser.setAcceptAllFileFilterUsed(false);
		chooser.addChoosableFileFilter(new FileNameExtensionFilter(
				"Troll Stomp Stage Files (*" + Constants.DOTTED_STAGE_FILE_EXTENSION() + ")",
				Constants.STAGE_FILE_EXTENSION));
		chooser.showOpenDialog(this);
	}

	void save() {
		try {
			File file = new File(stage.defaultSavePath());
			file.createNewFile();
			if (backgroundPath != null) {
				copyBackgroundImage();
			}
			saveToPath(new JsonWriter(new FileWriter(file)));
			Helpers.saveMessage(this);
		} catch (IOException e) {
			e.printStackTrace();
			Helpers.couldNotSaveFile(this);
		}
	}

	/**
	 * Checks if file already exists under approveSelection. The override code
	 * sourced from
	 * {@link http://stackoverflow.com/questions/3651494/jfilechooser-with-confirmation-dialog}
	 * and edited according to our needs.
	 */
	void saveAs() {

		JFileChooser chooser = Helpers.chooseFile(this);
		int returnVal = chooser.showSaveDialog(this);
		if (returnVal == JFileChooser.APPROVE_OPTION) {
			String savePath = chooser.getSelectedFile().getAbsolutePath();
			if (!savePath.endsWith(Constants.DOTTED_STAGE_FILE_EXTENSION())) {
				savePath += Constants.DOTTED_STAGE_FILE_EXTENSION();
			}
			try {
				saveToPath(new JsonWriter(new FileWriter(savePath)));
			} catch (IOException e) {
				e.printStackTrace();
				Helpers.couldNotSaveFile(this);
			}
		}
	}

	private void copyBackgroundImage() {
		SwingWorker<Boolean, Boolean> worker = new SwingWorker<Boolean, Boolean>() {

			boolean returnValue;

			@Override
			protected Boolean doInBackground() {
				try {
					if (stage.imageFormat == null) {
						stage.imageFormat = backgroundPath.substring(backgroundPath.lastIndexOf(".") + 1);
					}
					Files.copy(Paths.get(backgroundPath), Paths.get(stage.fullBackgroundPath()),
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
				if (!returnValue) {
					Helpers.couldNotCopyImage(getThis());
				}
			}

		};

		worker.execute();
	}

	private void saveToPath(JsonWriter writer) {
		String output = new Json(JsonWriter.OutputType.json).toJson(stage);
		if (output.length() == 0) {
			Helpers.couldNotSaveFile(this);
		} else {
			try {
				writer.write(output);
				writer.close();
			} catch (IOException e) {
				e.printStackTrace();
				Helpers.couldNotSaveFile(this);
			}
		}
	}

	private void setKeyboardShortcuts() {

		getRootPane().getInputMap().put(KeyStroke.getKeyStroke("F11"), "fullscreen");
		getRootPane().getActionMap().put("fullscreen", EditorActions.fullScreenAction(this));
		getRootPane().getInputMap().put(KeyStroke.getKeyStroke("F12"), "save as");
		getRootPane().getActionMap().put("save as", EditorActions.exportAction(this));
		getRootPane().getInputMap().put(
				KeyStroke.getKeyStroke(KeyEvent.VK_Z, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()), "undo");
		getRootPane().getActionMap().put("undo", EditorActions.undoAction(this));
		getRootPane().getInputMap().put(
				KeyStroke.getKeyStroke(KeyEvent.VK_Y, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()), "redo");
		getRootPane().getActionMap().put("redo", EditorActions.redoAction(this));
		getRootPane().getInputMap().put(
				KeyStroke.getKeyStroke(KeyEvent.VK_N, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()), "new");
		getRootPane().getActionMap().put("new", EditorActions.newAction());
		getRootPane().getInputMap().put(
				KeyStroke.getKeyStroke(KeyEvent.VK_O, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()), "open");
		getRootPane().getActionMap().put("open", EditorActions.openAction(this));
		getRootPane().getInputMap().put(
				KeyStroke.getKeyStroke(KeyEvent.VK_S, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()), "save");
		getRootPane().getActionMap().put("save", EditorActions.saveAction(this));

	}

	void redo() {
		// TODO Auto-generated method stub

	}

	void undo() {
		// TODO Auto-generated method stub

	}

	void changeFullscreen() {
		if (isUndecorated()) {
			setUndecorated(false);
		} else {
			setUndecorated(true);
		}
	}

	private void setNewBackgroundImage() {
		System.out.println(backgroundPath);
		try {
			bufferedImage = ImageIO.read(new File(backgroundPath));
			backgroundPanel.repaint();
		} catch (IOException e) {
			e.printStackTrace();
			JOptionPane.showMessageDialog(this, "Error recreating image");
		}
	}
	
	private void loadStaticObjectTypes() {
		Json jayson = new Json();
		try {
			staticObjectTypes = jayson.fromJson(StaticObjectTypes.class, new FileReader(new File(Paths.get(Constants.STATIC_OBJECT_CONSTANTS_FILE_PATH()).toString())));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			Helpers.couldNotOpenFile(this);
			staticObjectTypes = new StaticObjectTypes(new ArrayList<StaticObjectType>());
		}
	}

	/** Deselects the currently selected object by removing its outline. */
	public void deselect() {
		if (selectedIcon != null) {
			selectedIcon.setBorder(Constants.NO_BORDER);
			selectedIcon.menu.setVisible(false);
			selectedIcon = null;
		}
		sideMenu.setVisible(true);
	}

}
