package groop.shade.editor;

public class StaticObjectType implements StageType {

	private String name;
	private float scale;
	public String imageFormat;
	public String shadowImageFormat;
	
	StaticObjectType() {}
	
	StaticObjectType(String n, float s)
	{
		name = n;
		scale = s;
		imageFormat = null;
		shadowImageFormat = null;
	}
	
	StaticObjectType(String n) {
		name = n;
		scale = getDefaultScale();
		imageFormat = null;
		shadowImageFormat = null;
	}
	
	@Override
	public String getName() {
		return name;
	}

	@Override
	public EditorItem getValue() {
		return new StaticObjectEditorItem(this);
	}

	@Override
	public String getIconPath() {
		return Constants.STATIC_OBJECT_ASSETS_FILE_PATH() + getName() + "." + imageFormat;
	}

	@Override
	public String getIconShadowPath() {
		return Constants.STATIC_OBJECT_ASSETS_FILE_PATH() + getName() + "_S." + imageFormat;
	}

	public float getScale() {
		return scale;
	}

	@Override
	public float getDefaultScale() {
		return 1.0f;
	}
	
	

}
