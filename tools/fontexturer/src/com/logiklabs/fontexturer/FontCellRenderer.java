package com.logiklabs.fontexturer;

import java.awt.Component;
import java.awt.Font;

import javax.swing.DefaultListCellRenderer;
import javax.swing.JLabel;
import javax.swing.JList;

public class FontCellRenderer extends DefaultListCellRenderer {
	public Component getListCellRendererComponent(JList list, Object value,
			int index, boolean isSelected, boolean cellHasFocus) {
		JLabel label = (JLabel) super.getListCellRendererComponent(list, value,
				index, isSelected, cellHasFocus);
		String str = value.toString();
		Font font = new Font(str, Font.PLAIN, 14);
		if(font.canDisplayUpTo(str) == -1)
			label.setFont(font);
		else
			label.setFont(new Font(label.getFont().getFontName(), Font.PLAIN, 14));
		return label;
	}
}
