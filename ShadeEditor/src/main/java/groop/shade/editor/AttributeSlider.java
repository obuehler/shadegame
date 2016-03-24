package groop.shade.editor;

import java.awt.Dimension;
import java.util.Hashtable;

import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public abstract class AttributeSlider extends JPanel implements ChangeListener {

	private static final int MINOR_TICKS_PER_MAJOR_TICK = 5;

	public static class CustomSlider extends JSlider implements ChangeListener {
		public Integer lowerLimit;
		public Integer base;

		public CustomSlider(int min, int max, int b) {
			super(min * MINOR_TICKS_PER_MAJOR_TICK, max * MINOR_TICKS_PER_MAJOR_TICK);
			preInitialize();
			lowerLimit = null;
			base = b;
			setMinorTickSpacing(1);
			setMajorTickSpacing(MINOR_TICKS_PER_MAJOR_TICK);
			Hashtable<Integer, JLabel> labelTable = new Hashtable<Integer, JLabel>();
			for (int k = min; k <= max; k++) {
				if (k < 0) {
					labelTable.put(new Integer(k * MINOR_TICKS_PER_MAJOR_TICK),
							new JLabel("1/" + (int) Math.pow(base, -k)));
				} else {
					labelTable.put(new Integer(k * MINOR_TICKS_PER_MAJOR_TICK),
							new JLabel(Integer.toString((int) Math.pow(base, k))));
				}
			}
			setLabelTable(labelTable);
		}

		public CustomSlider(int min, int max) {
			super(min, max);
			preInitialize();
			lowerLimit = min;
			base = null;
			setMajorTickSpacing(Math.round((float) (max - min) / 4f));
			setMinorTickSpacing((int) Math.ceil((double) getMajorTickSpacing() / 5d));
			if ((double) (max / getMajorTickSpacing()) == (double) max / (double) getMajorTickSpacing()
					&& (double) (min / getMajorTickSpacing()) != (double) min / (double) getMajorTickSpacing()) {
				setMinimum((min / getMajorTickSpacing()) * getMajorTickSpacing());
				Hashtable<Integer, JLabel> labelTable = new Hashtable<Integer, JLabel>();
				for (int k = getMinimum(); k <= max; k = k + (MINOR_TICKS_PER_MAJOR_TICK * getMinorTickSpacing())) {
					if (k < lowerLimit) {
						labelTable.put(new Integer(k), new JLabel(Integer.toString(lowerLimit)));
					} else {
						labelTable.put(new Integer(k), new JLabel(Integer.toString(k)));
					}
				}
				setMajorTickSpacing(getMinorTickSpacing());
				setLabelTable(labelTable);
			}
		}

		protected void preInitialize() {
			setPaintLabels(true);
			setPaintTicks(true);
			addChangeListener(this);
		}

		@Override
		public void stateChanged(ChangeEvent arg0) {
			if (lowerLimit != null && getValue() < lowerLimit) {
				setValue(lowerLimit);
			}
		}

	}

	public CustomSlider slider;
	public JLabel titleLabel;
	public JLabel valueLabel;

	public AttributeSlider(int min, int max, String title) {
		slider = new CustomSlider(min, max);
		initialize(title);
	}

	public AttributeSlider(int min, int max, int b, String title) {
		slider = new CustomSlider(min, max, b);
		initialize(title);
	}

	private void initialize(String title) {
		slider.addChangeListener(this);
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		titleLabel = new JLabel(title);
		valueLabel = new JLabel();
		add(titleLabel);
		add(slider);
		add(valueLabel);
		setPreferredSize(new Dimension(200, 80));
		setMinimumSize(new Dimension(200, 60));
		setMaximumSize(new Dimension(2000, 80));
		setInitialValue();
		update();
	}

	public void update() {
		refreshText();
		updateAttribute();
	}

	public void refreshText() {
		if (slider.base == null) {
			valueLabel.setText(Integer.toString(slider.getValue()));
		} else {
			valueLabel.setText(Double.toString(
					Math.round(Math.pow(slider.base, (double) slider.getValue() / MINOR_TICKS_PER_MAJOR_TICK) * 100.0)
							/ 100.0));
		}
	}

	@Override
	public void stateChanged(ChangeEvent e) {
		update();
	}

	public abstract void setInitialValue();

	public abstract void updateAttribute();

}
