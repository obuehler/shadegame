package groop.shade.editor;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.text.NumberFormat;
import java.util.ArrayList;

import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JFormattedTextField;
import javax.swing.JRadioButton;
import javax.swing.JTextField;

import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.utils.NumberUtils;

import groop.shade.editor.ObjectSelector.SetTargetPositionButton;

@SuppressWarnings("serial")
public class TreeIcon extends StageObjectIcon {
	
	ObstacleType movingObjectType;
	
	/** These text fields store and display the pixel x and y values of
	 * the next action's target position, if added onto the list of actions. */
	JTextField targetFieldX;
	JTextField targetFieldY;
	
	public static final int INITIAL_INTEGER_VALUE = 0;
	public static final float INITIAL_FLOAT_VALUE = 0.0f;

	public TreeIcon(int x, int y, StageEditor s, ObstacleType t) {
		super(t, x, y, s);
	}

	public TreeIcon(TreeSite t, StageEditor s) {
		super(t, s);
	}

	protected void setDisplayIcon() {
		setDisplayIcon(movingObjectType.getIconPath(), movingObjectType.getIconShadowPath(),
				movingObjectType.getDefaultScale());
	}

	@Override
	public void delete() {
		editor.backgroundPanel.remove(this);
		getListFromStage().remove(object);
	}

	@Override
	public void changeEditor(StageEditor e) {
		getListFromStage().remove(object);
		editor = e;
		getListFromStage().add((TreeSite) object);
	}

	@Override
	protected void setObject(StageType e, int x, int y) {
		movingObjectType = (ObstacleType) e;
		setObject(x, y);
	}

	@Override
	protected void setObject(int x, int y) {
		object = new TreeSite(stagePosition(), 1.0f, new ArrayList<ActionMetadata>(), movingObjectType);
		getListFromStage().add((TreeSite) object);
	}

	@Override
	protected void refreshToolTipText() {
		setToolTipText(movingObjectType.getName());
	}

	@Override
	protected void setMenu() {
		/*final JComboBox<String> actionDropDownMenu = new JComboBox<String>(((TreeSite) object).actions);
		actionDropDownMenu.setEnabled(false);
		menu.add(actionDropDownMenu); */
		
		final ButtonGroup actionButtonGroup = new ButtonGroup();
		for (String s : ((TreeSite) object).actions) {
			JRadioButton button = new JRadioButton(s);
			button.setActionCommand(s);
			actionButtonGroup.add(button);
			menu.add(button);
		}
		
		JPanel headingPanel = new JPanel();
		headingPanel.setLayout(new BoxLayout(headingPanel, BoxLayout.LINE_AXIS));
		headingPanel.add(new JLabel("Heading: "));
		final JTextField headingField = new JTextField();
		headingField.setText(Float.toString(INITIAL_FLOAT_VALUE));
		headingField.setColumns(10);
		headingPanel.add(headingField);
		menu.add(headingPanel);
		
		JPanel lengthPanel = new JPanel();
		lengthPanel.setLayout(new BoxLayout(lengthPanel, BoxLayout.LINE_AXIS));
		lengthPanel.add(new JLabel("Length: "));
		final JTextField lengthField = new JTextField();
		lengthField.setText(Integer.toString(INITIAL_INTEGER_VALUE));
		lengthField.setColumns(10);
		lengthPanel.add(lengthField);
		menu.add(lengthPanel);
		
		JPanel counterPanel = new JPanel();
		counterPanel.setLayout(new BoxLayout(counterPanel, BoxLayout.LINE_AXIS));
		counterPanel.add(new JLabel("Counter: "));
		final JTextField counterField = new JTextField();
		counterField.setText(Integer.toString(INITIAL_INTEGER_VALUE));
		counterField.setColumns(10);
		counterPanel.add(counterField);
		menu.add(counterPanel);
		
		JPanel targetPanel = new JPanel();
		targetPanel.setLayout(new BoxLayout(targetPanel, BoxLayout.LINE_AXIS));
		targetPanel.add(new JLabel("Target position: "));
		targetFieldX = new JTextField();
		targetFieldX.setEditable(false);
		targetFieldX.setText("");
		targetFieldX.setColumns(4);
		targetFieldY = new JTextField();
		targetFieldY.setEditable(false);
		targetFieldY.setText("");
		targetFieldY.setColumns(4);
		targetPanel.add(targetFieldX);
		targetPanel.add(new JLabel(", "));
		targetPanel.add(targetFieldY);
		menu.add(targetPanel);
		menu.add(new SetTargetPositionButton(editor).button);
		
		JButton removeTargetButton = new JButton("Remove Target");
		removeTargetButton.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				targetFieldX.setText("");
				targetFieldY.setText("");
				editor.targetIcon.setVisible(false);
			}
			
		});
		menu.add(removeTargetButton);
		
		final CyclicButtonGroup cyclicButtons = new CyclicButtonGroup();
		
		JRadioButton nonCyclicButton = new JRadioButton("Set to non-cyclic");
		nonCyclicButton.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				try {
					cyclicButtons.beginningOfCycle.data.cycleStart = false;
					cyclicButtons.beginningOfCycle = null;
				} catch (NullPointerException e) {
					e.printStackTrace();
				}
			}
			
		});
		cyclicButtons.add(nonCyclicButton);
		
		final JButton addActionButton = new JButton("Add Action");
		
		menu.add(addActionButton);
		menu.add(nonCyclicButton);
		
		addActionButton.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				if (actionButtonGroup.getSelection() == null) {
					JOptionPane.showMessageDialog(null, "Please select an action type");
					return;
				}
				Float headingValue = null;
				Integer lengthValue = null;
				Integer counterValue = null;
				Integer targetValueX = null;
				Integer targetValueY = null;
				boolean noTarget = false;
				try {
					targetValueX = Integer.parseInt(targetFieldX.getText());
					targetValueY = Integer.parseInt(targetFieldY.getText());
				} catch (NumberFormatException nfe) {
					noTarget = true;
				}
				if (headingField.getText() == null || !isValidHeading(headingField.getText())) {
					if (noTarget) {
						JOptionPane.showMessageDialog(null, "Please enter a heading value greater than or equal to 0 and less than 360");
						return;
					}
				} else {
					headingValue = Float.parseFloat(headingField.getText());
				}
				if (lengthField.getText() == null || !isValidLength(lengthField.getText())) {
					if (noTarget) {
						JOptionPane.showMessageDialog(null, "Please enter an integer length value greater than 0");
						return;
					}
				} else {
					lengthValue = Integer.parseInt(lengthField.getText());
				}
				if (counterField.getText() == null || !isValidCounter(counterField.getText(), Integer.parseInt(lengthField.getText()))) {
					if (noTarget) {
						JOptionPane.showMessageDialog(null, "Please enter an integer counter value greater than 0 and less than the length value");
						return;
					}
				} else {
					counterValue = Integer.parseInt(counterField.getText());
				}	
				
				ActionMetadata data = new ActionMetadata(actionButtonGroup.getSelection().getActionCommand(),
						headingValue, lengthValue, counterValue, false, targetValueX, targetValueY);
				((TreeSite) object).actionQueue.add(data);
				createActionPanel(cyclicButtons, data);
			}
			
		});
		
		for (ActionMetadata metadata : ((TreeSite) object).actionQueue) {
			createActionPanel(cyclicButtons, metadata);
		}
		
	}
	
	protected void createActionPanel(final CyclicButtonGroup cyclicButtons,
			ActionMetadata data) {
		ActionMetadataPanel mainActionPanel = new ActionMetadataPanel(data, cyclicButtons, ((TreeSite) object).actionQueue, menu);
		menu.add(mainActionPanel);
		mainActionPanel.setVisible(true);
		menu.setVisible(false);
		menu.setVisible(true);
	}
	
	boolean isValidHeading(String heading) {
		try {
			float actualHeading = Float.parseFloat(heading);
			return (actualHeading >= 0.0f || actualHeading < 360.0f);
		} catch (NumberFormatException e) {
			return false;
		}
	}
	
	boolean isValidLength(String length) {
		try {
			int actualLength = Integer.parseInt(length);
			return (actualLength > 0);
		} catch (NumberFormatException e) {
			return false;
		}
	}
	
	boolean isValidCounter(String counter, int length) {
		try {
			int actualCounter = Integer.parseInt(counter);
			return (actualCounter > 0 && actualCounter <= length);
		} catch (NumberFormatException e) {
			return false;
		}
	}
	
	ArrayList<TreeSite> getListFromStage() {
		switch (movingObjectType) {
		case PEDESTRIAN:
			return editor.stage.pedestrians;
		case CAR:
			return editor.stage.cars;
		default:
			return null;
		}
	}

	@Override
	protected void setObject(Vector2 o) {
		super.setObject(o);
		movingObjectType = ((TreeSite) o).type;
	}

}
