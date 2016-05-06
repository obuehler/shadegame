package groop.shade.editor;

import java.io.File;

import javax.imageio.ImageIO;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.filechooser.FileNameExtensionFilter;

@SuppressWarnings("serial")
public class Helpers {

	public static int fileExistsDialog(StageEditor s) {
		int result = JOptionPane.showConfirmDialog(s,
				"A file with that name already exists in this folder. Overwrite file?", "Overwrite File?",
				JOptionPane.YES_NO_CANCEL_OPTION);
		return result;
	}

	public static void couldNotOpenFile(StageEditor s) {
		JOptionPane.showMessageDialog(s, "Could not open file");
	}
	
	public static void couldNotLoadStaticObjectTypes(StageEditor s) {
		JOptionPane.showMessageDialog(s, "Could not load static object types");
	}

	public static void couldNotCopyImage(StageEditor s) {
		JOptionPane.showMessageDialog(s, "Warning: Could not copy background image into the \"backgrounds\" directory");
	}
	
	public static void couldNotCopyObjectImage(StageEditor s) {
		JOptionPane.showMessageDialog(s, "Could not copy object image into the \"static_objects\" directory");
	}

	public static void couldNotSaveFile(StageEditor s) {
		JOptionPane.showMessageDialog(s, "Could not save file");
	}
	
	public static void couldNotSaveStaticObjectTypes(StageEditor s) {
		JOptionPane.showMessageDialog(s, "Could not save static object types");
	}

	public static int confirmQuit(StageEditor s) {
		return JOptionPane.showConfirmDialog(s, "Do you want to save your changes before you quit?", "Quit",
				JOptionPane.YES_NO_CANCEL_OPTION);
	}

	public static void couldNotExportImage(StageEditor s) {
		JOptionPane.showMessageDialog(s, "Could not export image");
	}

	public static int confirmNewEditor(StageEditor s) {
		return JOptionPane.showConfirmDialog(s,
				"Do you want to open a new editor? If so, click \"Yes.\" If you choose \"No,\", your changes to the current stage will be lost because Emre is a lazy ass and did not implement multiple stages in a single editor.",
				"Attention!", JOptionPane.YES_NO_CANCEL_OPTION);
	}

	public static void saveMessage(StageEditor s) {
		JOptionPane.showMessageDialog(s, "Saved into the game stages at " + s.stage.defaultSavePath());
	}

	public static JFileChooser chooseFile(final StageEditor s) {
		JFileChooser chooser = new JFileChooser() {

			@Override
			public void approveSelection() {
				File file = getSelectedFile();
				if (file != null && !file.getAbsolutePath().endsWith(Constants.DOTTED_STAGE_FILE_EXTENSION())) {
					file = new File(file.getAbsolutePath() + Constants.DOTTED_STAGE_FILE_EXTENSION());
				}
				if (file.exists() && getDialogType() == SAVE_DIALOG) {
					int result = Helpers.fileExistsDialog(s);
					switch (result) {
					case JOptionPane.YES_OPTION:
						super.approveSelection();
						return;
					case JOptionPane.CANCEL_OPTION:
						cancelSelection();
						return;
					default:
						return;
					}
				}
				super.approveSelection();
			}
		};

		chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
		return chooser;
	}

	/*public static int openDialog(JFileChooser chooser) {
		return chooser.showOpenDialog(null);
	}*/

	public static void openImageSelectionScreen(JFileChooser chooser) {
		chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
		chooser.setAcceptAllFileFilterUsed(false);

		/*
		 * Code to add image filter to file chooser obtained from
		 * 
		 * @link
		 * http://stackoverflow.com/questions/16303868/jfilechooser-that
		 * -will-only-select-java-supported-image-file-formats
		 */

		// Get array of available formats
		String[] suffixes = ImageIO.getReaderFileSuffixes();
		String displaySuffixes = "Image files";
		for (String s : suffixes) {
			displaySuffixes = displaySuffixes.concat(" (*." + s + ")");
		}
		// Add a file filter for each one
		chooser.addChoosableFileFilter(new FileNameExtensionFilter(displaySuffixes, suffixes));
		chooser.showOpenDialog(null);
	}
	
	public static String relativeSavePath(String name) {
		return Constants.STAGES_PATH() + name + Constants.DOTTED_STAGE_FILE_EXTENSION();
	}

}
