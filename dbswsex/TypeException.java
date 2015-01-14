package com.db.sws.dbswsex;

public class TypeException extends com.db.sws.dbswsex.DBSWSException {

	private static final long serialVersionUID = -7906310225048606491L;

	public TypeException() {
		super(SEVERITY_ERROR, FACILITY_GENERAL, GEN_TYPEMISMATCH);
	}
}
