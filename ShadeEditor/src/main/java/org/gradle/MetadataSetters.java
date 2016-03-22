package org.gradle;

import java.awt.Dimension;
import java.io.File;
import java.util.ArrayList;

import javax.swing.JOptionPane;

public class MetadataSetters {
	
	public static final int MIN_STAGE_WIDTH = 400;
	public static final int MIN_STAGE_HEIGHT = 400;
	public static final int MAX_STAGE_WIDTH = 20000;
	public static final int MAX_STAGE_HEIGHT = 20000;

	public static Dimension inputSize() {
		return inputSize("");
	}
	
	/** Edits stage size, prompting user input with a dialog box.
	 *  Accepts all strings as long as there are two numbers with some kind of 
	 *  separation between them. Then checks whether the numbers fall within
	 *  the range of (MIN_WIDTH, MIN_HEIGHT) to (MAX_WIDTH, MAX_HEIGHT) 
	 * @return new stage size
	 */
	private static Dimension inputSize(String preExisting) {
		ArrayList<Character> widthChars = new ArrayList<Character>();
		ArrayList<Character> heightChars = new ArrayList<Character>();
		ArrayList<ArrayList<Character>> lists = new ArrayList<ArrayList<Character>>();
		lists.add(widthChars);
		lists.add(heightChars);
		try {
			String inputString = JOptionPane.showInputDialog("Enter the size of the map.", preExisting);
			if (inputString.length() == 0) return invalidSize(inputString);
			char[] chars = inputString.toCharArray();
			// If user presses cancel or quits, chars == null and NullPointerException occurs.
			int index = 0;
			lbl:
				for (int listIndex = 0; listIndex < 2; listIndex++) {
					while (!Character.isDigit(chars[index])) {
						index++;
						if (index >= chars.length) { break lbl; }
					}
					while (Character.isDigit(chars[index])) {
						lists.get(listIndex).add(chars[index]);
						index++;
						if (index >= chars.length) { break lbl; }
					}
				}
			if (widthChars.size() == 0 || heightChars.size() == 0) {
				return invalidSize(inputString);
			} else {
				try {
					int width = getInputValue(widthChars);
					int height = getInputValue(heightChars);
					if (width > MAX_STAGE_WIDTH || height > MAX_STAGE_HEIGHT || width < MIN_STAGE_WIDTH || height < MIN_STAGE_HEIGHT) {
						return invalidSize(inputString);	
					} else {
						return new Dimension(width, height);
					}
				} catch (NumberFormatException e) {
					return invalidSize(inputString);
				}
			}
		} catch (NullPointerException e) {
			return null;
		}
	}

	private static int getInputValue(ArrayList<Character> widthChars) {
		StringBuilder builder = new StringBuilder(widthChars.size());
		for (char c : widthChars) {
			builder.append(c);
		}
		return Integer.parseInt(builder.toString());
	}
	
	private static Dimension invalidSize(String preExisting) {
		int i = JOptionPane.showConfirmDialog(null, 
			"Please enter a valid size smaller than (" + 
			Integer.toString(MAX_STAGE_WIDTH) + ", " +
			Integer.toString(MAX_STAGE_HEIGHT) + ") and larger than (" + 
			Integer.toString(MIN_STAGE_WIDTH) + ", " +
			Integer.toString(MIN_STAGE_HEIGHT) + ").", "Invalid Size",
			JOptionPane.OK_CANCEL_OPTION);
		if (i == JOptionPane.OK_OPTION) {
			return inputSize(preExisting);
		} else {
			return null;
		}
	}
	
	/** Used to edit the stage name.
	 * 
	 * @return the new stage name (possibly)
	 */
	public static String inputName() {
		return inputName("");
	}
	
	private static String inputName(String preExisting) {
		String name = JOptionPane.showInputDialog("Enter stage name.", preExisting);
		/* If user selects "Cancel" or quits, name is null.
		/* This must be handled in the function that called this. */
		if (name == null) {
			return name;
		} else if (name.length() == 0) {
			return emptyName();
		} else if (new File(name).exists()){
			return fileAlreadyExists(name);
		} else {
			return name;
		}
	}

	private static String emptyName() {
		int i = JOptionPane.showConfirmDialog(null, "Please enter a name.", "Invalid Name", JOptionPane.OK_CANCEL_OPTION);
		if (i == JOptionPane.OK_OPTION) {
			return inputName("");
		} else {
			return null;
		}
	}
	
	private static String fileAlreadyExists(String name) {
		switch(Helpers.fileExistsDialog(null)) {
	        case JOptionPane.YES_OPTION:
	            return name;
	        case JOptionPane.NO_OPTION:
	        	return inputName(name);
	        default:
	        	return null;
		}
    }
	
}
