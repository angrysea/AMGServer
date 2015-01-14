package com.db.sws.dbswsex;

public class DBSWSException extends BaseException {

	private static final long serialVersionUID = -1400441403453456334L;

	public DBSWSException(int hr) {
		super(hr);
	}

	public DBSWSException(int sev, int fac, int code) {
		super(sev, fac, code);
	}

	public DBSWSException(int sev, int fac, int code, String msg) {
		super(sev, fac, code, msg);
	}

	public DBSWSException(int sev, int code) {
		super(sev, code);
	}

	public void logMessage(String msg) {
		try {
			if (bVerbose == true) {
				System.out.println("LogEntry:");
				System.out.println("\tseverity="
						+ DBSWSException.getSeverityText(getSeverity()));
				System.out.println("\tfacility="
						+ DBSWSException.getFacilityText(getFacility()));
				System.out.println("\terrorCode=" + getCode());
				System.out.println("\terrorMessage=" + getMessage());
				System.out.println("\textraText=" + msg);
				System.out.println("\tentryTime=" + exceptionDate.toString());
				System.out.println("");
			}
		} catch (Exception e) {
			System.err.println(e);
		}
	}

}
