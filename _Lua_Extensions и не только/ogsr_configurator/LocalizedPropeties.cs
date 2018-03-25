using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Globalization;
using System.Reflection;
using System.Drawing.Design;
using System.Diagnostics;
using System.Resources;

namespace Configurator
{
    internal sealed class LocalizedDescriptionAttribute : DescriptionAttribute
    {
        private readonly string resourceName;
        public LocalizedDescriptionAttribute(string resourceName)
            : base()
        {
            this.resourceName = resourceName;
        }

        public override string Description
        {
            get
            {
                ResourceManager res = Form1.GetResourceManager();
                if (res != null)
                    return res.GetString(this.resourceName);
                return this.resourceName;
            }
        }
    }

    [AttributeUsage(AttributeTargets.Property, AllowMultiple = false, Inherited = true)]
    public class GlobalizedPropertyAttribute : Attribute
    {
        private String resourceName = "";
        private String resourceDescription = "";
        private String resourceCategory = "";

        public GlobalizedPropertyAttribute(String name)
        {
            resourceName = name;
        }

        public String Name
        {
            get { return resourceName; }
            set { resourceName = value; }
        }

        public String Description
        {
            get { return resourceDescription; }
            set { resourceDescription = value; }
        }

        public String Category
        {
            get { return resourceCategory; }
            set { resourceCategory = value; }
        }

    }

    public class GlobalizedPropertyDescriptor : PropertyDescriptor
    {
        private PropertyDescriptor basePropertyDescriptor;
        private String localizedName = "";
        private String localizedDescription = "";
        private String localizedCategory = "";

        public GlobalizedPropertyDescriptor(PropertyDescriptor basePropertyDescriptor)
            : base(basePropertyDescriptor)
        {
            this.basePropertyDescriptor = basePropertyDescriptor;
        }

        public override bool CanResetValue(object component)
        {
            return basePropertyDescriptor.CanResetValue(component);
        }

        public override Type ComponentType
        {
            get { return basePropertyDescriptor.ComponentType; }
        }

        public override string DisplayName
        {
            get
            {
                // First lookup the property if GlobalizedPropertyAttribute instances are available. 
                // If yes, then try to get resource table name and display name id from that attribute.
                string displayName = "";
                foreach (Attribute oAttrib in this.basePropertyDescriptor.Attributes)
                {
                    if (oAttrib.GetType().Equals(typeof(GlobalizedPropertyAttribute)))
                        displayName = ((GlobalizedPropertyAttribute)oAttrib).Name;
                }

                // If no display name id is specified by attribute, then construct it by using default display name (usually the property name) 
                if (displayName.Length == 0)
                    displayName = this.basePropertyDescriptor.DisplayName;

                // Get the string from the resources. 
                // If this fails, then use default display name (usually the property name) 
                string s = Form1.GetResourceManager().GetString(displayName);
                this.localizedName = (s != null) ? s : this.basePropertyDescriptor.DisplayName;

                return this.localizedName;
            }
        }

        public override string Description
        {
            get
            {
                // First lookup the property if there are GlobalizedPropertyAttribute instances
                // are available. 
                // If yes, try to get resource table name and display name id from that attribute.
                string displayName = "";
                foreach (Attribute oAttrib in this.basePropertyDescriptor.Attributes)
                {
                    if (oAttrib.GetType().Equals(typeof(GlobalizedPropertyAttribute)))
                        displayName = ((GlobalizedPropertyAttribute)oAttrib).Description;
                }

                // If no display name id is specified by attribute, then construct it by using default display name (usually the property name) 
                if (displayName.Length == 0)
                    displayName = this.basePropertyDescriptor.DisplayName + "Description";

                // Get the string from the resources. 
                // If this fails, then use default empty string indictating 'no description' 
                string s = Form1.GetResourceManager().GetString(displayName);
                this.localizedDescription = (s != null) ? s : "";

                return this.localizedDescription;
            }
        }
        public override string Category
        {
            get
            {
                // First lookup the property if there are GlobalizedPropertyAttribute instances
                // are available. 
                // If yes, try to get resource table name and display name id from that attribute.
                string displayName = "";
                foreach (Attribute oAttrib in this.basePropertyDescriptor.Attributes)
                {
                    if (oAttrib.GetType().Equals(typeof(GlobalizedPropertyAttribute)))
                        displayName = ((GlobalizedPropertyAttribute)oAttrib).Category;
                }

                // If no display name id is specified by attribute, then construct it by using default display name (usually the property name) 
                if (displayName.Length == 0)
                    displayName = this.basePropertyDescriptor.DisplayName + "Category";

                // Get the string from the resources. 
                // If this fails, then use default empty string indictating 'no description' 
                string s = Form1.GetResourceManager().GetString(displayName);
                this.localizedCategory = (s != null) ? s : "";

                return this.localizedCategory;
            }
        }

        public override object GetValue(object component)
        {
            return this.basePropertyDescriptor.GetValue(component);
        }

        public override bool IsReadOnly
        {
            get { return this.basePropertyDescriptor.IsReadOnly; }
        }

        public override string Name
        {
            get { return this.basePropertyDescriptor.Name; }
        }

        public override Type PropertyType
        {
            get { return this.basePropertyDescriptor.PropertyType; }
        }

        public override void ResetValue(object component)
        {
            this.basePropertyDescriptor.ResetValue(component);
        }

        public override bool ShouldSerializeValue(object component)
        {
            return this.basePropertyDescriptor.ShouldSerializeValue(component);
        }

        public override void SetValue(object component, object value)
        {
            this.basePropertyDescriptor.SetValue(component, value);
        }
    }

    public class GlobalizedObject : ICustomTypeDescriptor
    {
        private PropertyDescriptorCollection globalizedProps;

        public String GetClassName()
        {
            return TypeDescriptor.GetClassName(this, true);
        }

        public AttributeCollection GetAttributes()
        {
            return TypeDescriptor.GetAttributes(this, true);
        }

        public String GetComponentName()
        {
            return TypeDescriptor.GetComponentName(this, true);
        }

        public TypeConverter GetConverter()
        {
            return TypeDescriptor.GetConverter(this, true);
        }

        public EventDescriptor GetDefaultEvent()
        {
            return TypeDescriptor.GetDefaultEvent(this, true);
        }

        public PropertyDescriptor GetDefaultProperty()
        {
            return TypeDescriptor.GetDefaultProperty(this, true);
        }

        public object GetEditor(Type editorBaseType)
        {
            return TypeDescriptor.GetEditor(this, editorBaseType, true);
        }

        public EventDescriptorCollection GetEvents(Attribute[] attributes)
        {
            return TypeDescriptor.GetEvents(this, attributes, true);
        }

        public EventDescriptorCollection GetEvents()
        {
            return TypeDescriptor.GetEvents(this, true);
        }

        /// <summary>
        /// Called to get the properties of a type.
        /// </summary>
        /// <param name="attributes"></param>
        /// <returns></returns>
        public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
        {
            if (globalizedProps == null)
            {
                // Get the collection of properties
                PropertyDescriptorCollection baseProps = TypeDescriptor.GetProperties(this, attributes, true);

                globalizedProps = new PropertyDescriptorCollection(null);

                // For each property use a property descriptor of our own that is able to be globalized
                foreach (PropertyDescriptor oProp in baseProps)
                {
                    globalizedProps.Add(new GlobalizedPropertyDescriptor(oProp));
                }
            }
            return globalizedProps;
        }

        public PropertyDescriptorCollection GetProperties()
        {
            // Only do once
            if (globalizedProps == null)
            {
                // Get the collection of properties
                PropertyDescriptorCollection baseProps = TypeDescriptor.GetProperties(this, true);
                globalizedProps = new PropertyDescriptorCollection(null);

                // For each property use a property descriptor of our own that is able to be globalized
                foreach (PropertyDescriptor oProp in baseProps)
                {
                    globalizedProps.Add(new GlobalizedPropertyDescriptor(oProp));
                }
            }
            return globalizedProps;
        }

        public object GetPropertyOwner(PropertyDescriptor pd)
        {
            return this;
        }
    }
}
