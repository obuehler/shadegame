package groop.shade.editor;

public class StaticObjectType implements StageType {

	private String name;
	private double scale;
	public String imageFormat;
	public String shadowImageFormat;
	
	StaticObjectType() {}
	
	StaticObjectType(String n, double s)
	{
		name = n;
		scale = s;
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

	@Override
	public double getDefaultScale() {
		return scale;
	}
	
	

}
