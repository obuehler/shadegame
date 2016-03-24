package groop.shade.editor;

import java.awt.Dimension;

import javax.swing.ButtonGroup;
import javax.swing.JMenu;

@SuppressWarnings("serial")
public class BossSiteIcon extends StageIcon implements ChangeableIcon {

	public BossSiteIcon(BossType e, int x, int y, StageEditor s) {
		super(e, x, y, s);
	}

	public BossSiteIcon(BossSite b, StageEditor s) {
		super(b, s);
	}

	@Override
	protected void setDisplayIcon() {
		setDisplayIcon(((BossSite) object).type.getIconPath());
	}

	public void changeType(StageType e) {
		((BossSite) object).type = (BossType) e;
		setDisplayIcon();
		refreshMenu();
		refreshToolTipText();
	}

	@Override
	public void delete() {
		editor.stage.bossSites.remove(object);
		object = null;
		editor.backgroundPanel.remove(this);
	}

	@Override
	public void changeEditor(StageEditor e) {
		if (editor.selectedIcon == this) {
			EditorItems.deselect(editor);
		}
		editor.stage.bossSites.remove(object);
		editor = e;
		editor.stage.bossSites.add(((BossSite) object));
		EditorItems.deselect(editor);
		select();
	}

	@Override
	protected void refreshToolTipText() {
		setToolTipText(((BossSite) object).type.getName());
	}

	@Override
	protected void setObject(StageType e, int x, int y) {
		object = new BossSite(stagePosition(), (BossType) e, 1f);
		editor.stage.bossSites.add(((BossSite) object));
	}

	@Override
	protected void setObject(int x, int y) {
	} // Never called

	@Override
	protected void setMenu() {
		AttributeSlider attributeSlider = new AttributeSlider(1, 100, "Health") {

			@Override
			public void setInitialValue() {
				if (((BossSite) object).bossHealth > 0) {
					slider.setValue(((BossSite) object).bossHealth);
				} else {
					slider.setValue(GameStage.DEFAULT_BOSS_HEALTH);
				}
			}

			@Override
			public void updateAttribute() {
				((BossSite) object).bossHealth = slider.getValue();
			}

		};
		menu.add(attributeSlider);
		AttributeSlider attributeSlider2 = new AttributeSlider(-3, 3, 2, "Speed") {

			@Override
			public void setInitialValue() {
				if (((BossSite) object).bossSpeed > 0) {
					slider.setValue(Math.round((float) (Math.log(((BossSite) object).bossSpeed)
							/ ((Math.log(slider.base)) / (Math.log(5.0))))));
				} else {
					slider.setValue(0);
				}
			}

			@Override
			public void updateAttribute() {
				((BossSite) object).bossSpeed = (float) Math.pow(slider.base, slider.getValue() / 5.0);
			}

		};
		menu.add(attributeSlider2);
		ButtonGroup group = new ButtonGroup();
		JMenu typeMenu = new JMenu("Type                  >");
		typeMenu.setPreferredSize(new Dimension(200, 40));
		typeMenu.setSize(typeMenu.getPreferredSize());
		typeMenu.setMaximumSize(new Dimension(2000, 50));
		for (BossType bossType : BossType.values()) {
			EditorRadioButton button = new EditorRadioButton(bossType, this);
			if (bossType == ((BossSite) object).type)
				button.setSelected(true);
			group.add(button);
			typeMenu.add(button);
		}
		menu.add(typeMenu);
	}

	@Override
	public void refreshMenu() {
		// TODO Auto-generated method stub

	}

}
