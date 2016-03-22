package org.gradle;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JMenu;
import javax.swing.JMenuItem;

import org.gradle.ObjectSelector.CustomMenuItem;


public class EditorMenu extends JMenu {

	public static EditorMenu obstacleMenu(StageEditor s) {
		EditorMenu menu = new EditorMenu("Obstacle");
		for (ObstacleType obstacleType : ObstacleType.values()) { 
			menu.add(new CustomMenuItem(obstacleType, s));
		}
		return menu;
	}

	public static EditorMenu minionMenu(StageEditor s) {
		EditorMenu menu = new EditorMenu("Camp Site");
		for (MinionType minionType : MinionType.values()) { 
			menu.add(new CustomMenuItem(minionType, s));
		}
		return menu;
	}

	public static EditorMenu bossMenu(StageEditor s) {
		EditorMenu menu = new EditorMenu("Boss");
		for (BossType bossType : BossType.values()) { 
			menu.add(new CustomMenuItem(bossType, s));
		}
		// Add more bosses
		return menu;
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
		// Save map(you can save into any directory, the coders will insert it manually in the game), export image, open, new, exit
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
